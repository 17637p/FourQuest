#include "ResourceSystem.h"

#include "spdlog/spdlog.h"

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

	// ���ҽ� �ε� 
	auto pool = game_module::ThreadPool::GetInstance();

	// Model �ε�
	for (const auto& modelPath : list.modelPaths)
	{
		pool->EnqueueJob([this, modelPath]()
			{
				LoadModelResource(modelPath);
			});
	}

	// Animation �ε�
	for (const auto& animationPath : list.animationPaths)
	{
		pool->EnqueueJob([this, animationPath]()
			{
				LoadAnimation(animationPath);
			});
	}

	// Material �ε�
	for (const auto& materialPath : list.materialPaths)
	{
		pool->EnqueueJob([this, materialPath]()
			{
				LoadMaterial(materialPath);
			});
	}

}

void fq::game_engine::ResourceSystem::Finalize()
{

}

void fq::game_engine::ResourceSystem::LoadModelResource(const Path& modelPath)
{
	{
		std::shared_lock<Mutex> lock(mModelMutex);

		if (mModels.find(modelPath) != mModels.end())
		{
			return;
		}
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
			std::unique_lock<Mutex> lock(mStaticMeshMutex);
			mStaticMeshes.insert({ modelPath + mesh.Name, staticMesh });
		}
		else
		{
			auto skinnedMesh = mGameProcess->mGraphics->CreateSkinnedMesh(mesh);
			std::unique_lock<Mutex> lock(mSkinnedMeshMutex);
			mSkinnedMeshes.insert({ modelPath + mesh.Name, skinnedMesh });
		}
	}

	std::unique_lock<Mutex> lock(mModelMutex);
	mModels.insert({ modelPath, std::move(model) });
}

const fq::common::Model& fq::game_engine::ResourceSystem::GetModel(const Path& path)const
{
	std::shared_lock<Mutex> lock(mModelMutex);
	return mModels.find(path)->second;
}


void fq::game_engine::ResourceSystem::LoadAnimation(const Path& path)
{
	{
		std::shared_lock<Mutex> lock(mAnimationMutex);

		if (mAnimations.find(path) != mAnimations.end())
		{
			return;
		}
	}

	auto animationClip = mGameProcess->mGraphics->ReadAnimation(path);
	auto animation = mGameProcess->mGraphics->CreateAnimation(animationClip);

	std::unique_lock<Mutex> lock(mAnimationMutex);
	mAnimations.insert({ path, animation });
}

void fq::game_engine::ResourceSystem::LoadMaterial(const Path& path)
{
	std::unique_lock<Mutex> lock(mAnimationMutex);

	if (mAnimations.find(path) != mAnimations.end())
	{
		return;
	}

	auto materialInfo = mGameProcess->mGraphics->ReadMaterialInfo(path);
	auto material = mGameProcess->mGraphics->CreateMaterial(materialInfo);
	mMaterials.insert({ path, material });
}

void fq::game_engine::ResourceSystem::LoadAnimator(fq::game_module::Animator* animator)
{
	auto controllerPath = animator->GetControllerPath();
	auto nodeHierarchyPath = animator->GetNodeHierarchyPath();

	if (!std::filesystem::exists(controllerPath))
	{
		spdlog::warn("{} animation controller load fail", controllerPath);
		return;
	}
	if (!std::filesystem::exists(nodeHierarchyPath))
	{
		spdlog::warn("{} animation controller load fail", controllerPath);
		return;
	}

	// ��Ʈ�ѷ� �ε� 
	auto controller = mLoader.Load(controllerPath);

}

void fq::game_engine::ResourceSystem::SaveSceneResourceList(const std::filesystem::path& path)
{
	auto filePath = path / "resource_list.txt";

	SceneResourceList list;

	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	// Scene ���ҽ� ����
	for (auto& object : scene->GetObjectView())
	{
		SaveObjectResource(list, &object);
	}

	mGameProcess->mPrefabManager->LoadPrefabResource(scene);
	const auto& container = mGameProcess->mPrefabManager->GetPrefabContainer();

	// Prefab ���ҽ� ����
	for (auto& [id, instance] : container)
	{
		for (const auto& object : instance)
		{
			SaveObjectResource(list, object.get());
		}
	}

	// AnimatorController ���� Animation ����
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

		// �� ��� ���� 
		if (fs::exists(modelPath))
		{
			list.modelPaths.insert(modelPath);
		}

		// ���׸��� ����
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

		// �� ��� ���� 
		if (fs::exists(modelPath))
		{
			list.modelPaths.insert(modelPath);
		}

		// ���׸��� ����
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

