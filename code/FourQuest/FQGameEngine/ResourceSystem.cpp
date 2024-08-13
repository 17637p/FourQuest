#include "ResourceSystem.h"

#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Animator.h"
#include "../FQCommon/FQPath.h"
#include "GameProcess.h"

fq::game_engine::ResourceSystem::ResourceSystem()
	:mGameProcess(nullptr)
{}

fq::game_engine::ResourceSystem::~ResourceSystem()
{

}

void fq::game_engine::ResourceSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mPreOnLoadSceneHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::PreOnLoadScene>(this, &ResourceSystem::LoadSceneResource);
}

void fq::game_engine::ResourceSystem::LoadSceneResource(fq::event::PreOnLoadScene event)
{
	auto listPath = fq::path::GetScenePath() / event.sceneName / "resource_list.txt";

	SceneResourceList list;
	list.Load(listPath.string());

	// 리소스 로드 
	auto pool = game_module::ThreadPool::GetInstance();

	// Model 로드
	for (const auto& modelPath : list.modelPaths)
	{
		LoadModelResource(modelPath);
	}

	// Animation 로드
	for (const auto& animationPath : list.animationPaths)
	{
		LoadAnimation(animationPath);
	}

	//// Material 로드
	//for (const auto& materialPath : list.materialPaths)
	//{
	//	LoadMaterial(materialPath);
	//}
}

void fq::game_engine::ResourceSystem::Finalize()
{

}

void fq::game_engine::ResourceSystem::LoadModelResource(const Path& modelPath)
{
	if (mModels.find(modelPath) != mModels.end())
	{
		return;
	}

	auto model = mGameProcess->mGraphics->ReadModel(modelPath);

	for (const auto& [node, mesh] : model.Meshes)
	{
		if (mesh.Vertices.empty())
		{
			continue;
		}

		if (mesh.BoneVertices.empty())
		{
			auto staticMesh = mGameProcess->mGraphics->CreateStaticMesh(mesh);
			mStaticMeshes.insert({ modelPath + mesh.Name, staticMesh });
			mStaticMeshes.insert({ modelPath + node.Name, staticMesh }); // 유니티에서 메쉬이름이 노드이름으로 변환되어 해당 로직 추가합니다_홍지환
		}
		else
		{
			auto skinnedMesh = mGameProcess->mGraphics->CreateSkinnedMesh(mesh);
			mSkinnedMeshes.insert({ modelPath + mesh.Name, skinnedMesh });
			mSkinnedMeshes.insert({ modelPath + node.Name, skinnedMesh }); // 유니티에서 메쉬이름이 노드이름으로 변환되어 해당 로직 추가합니다_홍지환
		}
	}

	mModels.insert({ modelPath, std::move(model) });
}

const fq::common::Model& fq::game_engine::ResourceSystem::GetModel(const Path& path)const
{
	return mModels.find(path)->second;
}

void fq::game_engine::ResourceSystem::LoadAnimation(const Path& path)
{
	if (mAnimations.find(path) != mAnimations.end())
	{
		return;
	}

	auto animationClip = mGameProcess->mGraphics->ReadAnimation(path);
	auto animation = mGameProcess->mGraphics->CreateAnimation(animationClip);

	mAnimations.insert({ path, animation });
}

void fq::game_engine::ResourceSystem::LoadMaterial(const Path& path)
{
	if (mMaterials.find(path) != mMaterials.end())
	{
		return;
	}

	auto materialInfo = mGameProcess->mGraphics->ReadMaterialInfo(path);
	auto material = mGameProcess->mGraphics->CreateMaterial(materialInfo);
	mMaterials.insert({ path, material });
}

void fq::game_engine::ResourceSystem::SaveSceneResourceList(const std::filesystem::path& path)
{
	auto filePath = path / "resource_list.txt";

	SceneResourceList list;

	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	// Scene 리소스 쿼리
	for (auto& object : scene->GetObjectView())
	{
		SaveObjectResource(list, &object);
	}

	mGameProcess->mPrefabManager->LoadPrefabResource(scene);
	const auto& container = mGameProcess->mPrefabManager->GetPrefabContainer();

	// Prefab 리소스 쿼리
	for (auto& [id, instance] : container)
	{
		for (const auto& object : instance)
		{
			SaveObjectResource(list, object.get());
		}
	}

	// AnimatorController 에서 Animation 추출
	SaveAnimationResource(list);

	list.Save(filePath.string());
}

void fq::game_engine::ResourceSystem::SaveObjectResource(SceneResourceList& list, game_module::GameObject* object)
{
	using namespace fq::game_module;
	namespace fs = std::filesystem;

	if (object->HasComponent<SkinnedMeshRenderer>())
	{
		auto skinnedMeshRenderer = object->GetComponent<SkinnedMeshRenderer>();
		auto modelPath = skinnedMeshRenderer->GetModelPath();
		const auto& materialPaths = skinnedMeshRenderer->GetMaterialPaths();

		// 모델 경로 저장 
		if (fs::exists(modelPath))
		{
			list.modelPaths.insert(modelPath);
		}

		// 메테리얼 저장
		for (const auto& path : materialPaths)
		{
			if (fs::exists(path))
			{
				list.materialPaths.insert(path);
			}
		}
	}

	if (object->HasComponent<StaticMeshRenderer>())
	{
		auto staticMeshRenderer = object->GetComponent<StaticMeshRenderer>();
		auto modelPath = staticMeshRenderer->GetModelPath();
		const auto& materialPaths = staticMeshRenderer->GetMaterialPaths();

		// 모델 경로 저장 
		if (fs::exists(modelPath))
		{
			list.modelPaths.insert(modelPath);
		}

		// 메테리얼 저장
		for (const auto& path : materialPaths)
		{
			if (fs::exists(path))
			{
				list.materialPaths.insert(path);
			}
		}
	}

	if (object->HasComponent<Animator>())
	{
		auto animator = object->GetComponent<Animator>();
		auto controllerpath = animator->GetControllerPath();
		auto nodeHierachyPath = animator->GetNodeHierarchyPath();

		if (fs::exists(controllerpath))
		{
			list.animatorControllerPaths.insert(controllerpath);
		}
	}

}

void fq::game_engine::ResourceSystem::SaveAnimationResource(SceneResourceList& list)
{
	namespace fs = std::filesystem;

	for (const auto& path : list.animatorControllerPaths)
	{
		auto controller = mLoader.Load(path);

		for (const auto& state : controller->GetStateMap())
		{
			auto path = state.second.GetAnimationPath();

			if (fs::exists(path))
			{
				list.animationPaths.insert(path);
			}
		}
	}
}

bool fq::game_engine::ResourceSystem::HasModel(const Path& path) const
{
	return mModels.find(path) != mModels.end();
}

bool fq::game_engine::ResourceSystem::HasAnimation(const Path& path) const
{
	return mAnimations.find(path) != mAnimations.end();
}

std::shared_ptr<fq::graphics::IMaterial> fq::game_engine::ResourceSystem::GetMaterial(const Path& path) const
{
	auto iter = mMaterials.find(path);
	return iter == mMaterials.end() ? nullptr : iter->second;
}


std::shared_ptr<fq::graphics::IStaticMesh> fq::game_engine::ResourceSystem::GetStaticMesh(const Path& modelPath, std::string meshName) const
{
	auto iter = mStaticMeshes.find(modelPath + meshName);
	return iter == mStaticMeshes.end() ? nullptr : iter->second;
}

std::shared_ptr<fq::graphics::IAnimation> fq::game_engine::ResourceSystem::GetAnimation(const Path& path) const
{
	auto iter = mAnimations.find(path);
	return iter == mAnimations.end() ? nullptr : iter->second;
}

std::shared_ptr<fq::graphics::ISkinnedMesh> fq::game_engine::ResourceSystem::GetSkinnedMesh(const Path& modelPath, std::string meshName) const
{
	auto iter = mSkinnedMeshes.find(modelPath + meshName);
	return iter == mSkinnedMeshes.end() ? nullptr : iter->second;
}

