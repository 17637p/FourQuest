#include "ResourceSystem.h"

#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/UVAnimator.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQCommon/FQPath.h"
#include "GameProcess.h"

fq::game_engine::ResourceSystem::ResourceSystem()
	:mGameProcess(nullptr)
	, mLoadedResourceCount(0)
	, mResourceCount(0)
	, mbIsStopLoadResource(false)
{}

fq::game_engine::ResourceSystem::~ResourceSystem()
{

}

void fq::game_engine::ResourceSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;

	mLoadMaterialHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::LoadMaterial>(this, &ResourceSystem::LoadMaterialResource);

	mUnLoadSceneResourceHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::UnloadSceneResource>(this, &ResourceSystem::UnloadSceneResource);
}

void fq::game_engine::ResourceSystem::LoadSceneResource(fq::event::PreOnLoadScene event)
{
	auto listPath = fq::path::GetScenePath() / event.sceneName / "resource_list.txt";

	SceneResourceList list;
	list.Load(listPath.string());

	mResourceCount = list.modelPaths.size()
		+ list.animationPaths.size()
		+ list.materialPaths.size()
		+ list.uvAnimationPath.size()
		+ list.nodeHierachyPaths.size();
	mLoadedResourceCount = 0;

	// Model 로드
	for (const auto& modelPath : list.modelPaths)
	{
		if (mbIsStopLoadResource) return;
		LoadModelResource(modelPath);
		++mLoadedResourceCount;
	}

	// Animation 로드
	for (const auto& animationPath : list.animationPaths)
	{
		if (mbIsStopLoadResource) return;
		LoadAnimation(animationPath);
		++mLoadedResourceCount;
	}

	// UV Animation 로드 
	for (const auto& uvAnimationPath : list.uvAnimationPath)
	{
		if (mbIsStopLoadResource) return;
		LoadUVAnimation(uvAnimationPath);
		++mLoadedResourceCount;
	}

	// NodeHierachy 로드 
	for (const auto& nodeHierachyPath : list.nodeHierachyPaths)
	{
		if (mbIsStopLoadResource) return;
		LoadNodeHierarchy(nodeHierachyPath);
		++mLoadedResourceCount;
	}

	// Material 로드
	for (const auto& materialPath : list.materialPaths)
	{
		if (mbIsStopLoadResource) return;
		LoadMaterial(materialPath);
		++mLoadedResourceCount;
	}
}

void fq::game_engine::ResourceSystem::LoadMaterialResource(fq::event::LoadMaterial event)
{
	LoadMaterial(event.materialPath);
	*event.materialPtr = GetMaterial(event.materialPath);
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
			mStaticMeshes.insert({ modelPath + node.Name, staticMesh }); // 유니티에서 메쉬 이름이 노드 이름으로 변환되어 해당 로직 추가합니다_홍지환
		}
		else
		{
			auto skinnedMesh = mGameProcess->mGraphics->CreateSkinnedMesh(mesh);
			mSkinnedMeshes.insert({ modelPath + mesh.Name, skinnedMesh });
			mSkinnedMeshes.insert({ modelPath + node.Name, skinnedMesh }); // 유니티에서 메쉬 이름이 노드 이름으로 변환되어 해당 로직 추가합니다_홍지환
		}
	}

	mModels.insert({ modelPath, std::move(model) });
}

void fq::game_engine::ResourceSystem::UnloadModelResource(const Path& path)
{
	if (mModels.find(path) == mModels.end())
	{
		return;
	}

	auto& model = mModels.at(path);

	for (const auto& [node, mesh] : model.Meshes)
	{
		if (mesh.Vertices.empty())
		{
			continue;
		}

		if (mesh.BoneVertices.empty())
		{
			mStaticMeshes.erase(path + mesh.Name);
			mStaticMeshes.erase(path + node.Name); // 유니티에서 메쉬 이름이 노드 이름으로 변환되어 해당 로직 추가합니다_홍지환
		}
		else
		{
			mSkinnedMeshes.erase(path + mesh.Name);
			mSkinnedMeshes.erase(path + node.Name); // 유니티에서 메쉬 이름이 노드 이름으로 변환되어 해당 로직 추가합니다_홍지환
		}
	}
	mModels.erase(path);
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

void fq::game_engine::ResourceSystem::LoadUVAnimation(const Path& path)
{
	if (mUVAnimations.find(path) != mUVAnimations.end())
	{
		return;
	}

	auto uvAnimationInfo = mGameProcess->mGraphics->ReadUVAnimation(path);
	auto uvAnimation = mGameProcess->mGraphics->CreateUVAnimation(uvAnimationInfo);
	mUVAnimations.insert({ path, uvAnimation });
}

void fq::game_engine::ResourceSystem::LoadNodeHierarchy(const Path& path)
{
	if (mNodeHierarchies.find(path) != mNodeHierarchies.end())
	{
		return;
	}

	auto nodeHierarchyInfo = mGameProcess->mGraphics->ReadNodeHierarchy(path);
	auto nodeHierarchy = mGameProcess->mGraphics->CreateNodeHierarchy(nodeHierarchyInfo);
	mNodeHierarchies.insert({ path, nodeHierarchy });
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

		if (fs::exists(nodeHierachyPath))
		{
			list.nodeHierachyPaths.insert(nodeHierachyPath);
		}
	}

	if (object->HasComponent<UVAnimator>())
	{
		auto uvAnimator = object->GetComponent<UVAnimator>();
		auto uvAnimationPath = uvAnimator->GetUVAnimationPath();

		if (fs::exists(uvAnimationPath))
		{
			list.uvAnimationPath.insert(uvAnimationPath);
		}
	}

	if (object->HasComponent<ImageUI>())
	{
		auto imageUI = object->GetComponent<ImageUI>();

		for (auto& imageUIInfo : imageUI->GetUIInfomations())
		{
			if (fs::exists(imageUIInfo.ImagePath))
			{
				list.imageUIPaths.insert(imageUIInfo.ImagePath);
			}

			if (fs::exists(imageUIInfo.MaskPath))
			{
				list.imageUIPaths.insert(imageUIInfo.MaskPath);
			}
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

std::shared_ptr<fq::graphics::IUVAnimation> fq::game_engine::ResourceSystem::GetUVAnimation(const Path& path) const
{
	auto iter = mUVAnimations.find(path);
	return iter == mUVAnimations.end() ? nullptr : iter->second;
}

std::shared_ptr<fq::graphics::INodeHierarchy> fq::game_engine::ResourceSystem::GetNodeHierarchy(const Path& path) const
{
	auto iter = mNodeHierarchies.find(path);
	return iter == mNodeHierarchies.end() ? nullptr : iter->second;
}

std::shared_ptr<fq::graphics::ISkinnedMesh> fq::game_engine::ResourceSystem::GetSkinnedMesh(const Path& modelPath, std::string meshName) const
{
	auto iter = mSkinnedMeshes.find(modelPath + meshName);
	return iter == mSkinnedMeshes.end() ? nullptr : iter->second;
}

float fq::game_engine::ResourceSystem::GetLoadingRatio() const
{
	if (mResourceCount == 0)
	{
		return 0.f;
	}

	return static_cast<float>(mLoadedResourceCount) / mResourceCount;
}

void fq::game_engine::ResourceSystem::UnloadSceneResource(fq::event::UnloadSceneResource event)
{
	// 같은 씬의 경우에는 리소스를 언로드 하지 않습니다.
	if (event.currentSceneName == event.nextSceneName)
	{
		return;
	}

	// 다음 씬이 없는 경우에도 모든 리소스를 언로드합니다 .
	if (event.nextSceneName.empty())
	{
		mModels.clear();
		mStaticMeshes.clear();
		mSkinnedMeshes.clear();
		mNodeHierarchies.clear();
		mAnimations.clear();
		mUVAnimations.clear();
		mMaterials.clear();
		return;
	}

	auto currentScenePath = fq::path::GetScenePath() / event.currentSceneName / "resource_list.txt";
	SceneResourceList currentList;
	currentList.Load(currentScenePath.string());

	auto nextScenePath = fq::path::GetScenePath() / event.nextSceneName / "resource_list.txt";
	SceneResourceList nextList;
	nextList.Load(nextScenePath.string());

	// 다음씬에서 필요없는 리소스를 언로드합니다.

	// ModelResource
	auto& nextModels = nextList.modelPaths;

	for (auto& loadModel : currentList.modelPaths)
	{
		if (nextModels.find(loadModel) == nextModels.end())
		{
			UnloadModelResource(loadModel);
		}
	}

	// Animation
	auto& nextAnimations = nextList.animationPaths;
	
	for (auto& loadAnimation : currentList.animationPaths)
	{
		if (nextAnimations.find(loadAnimation) == nextAnimations.end())
		{
			UnloadAnimation(loadAnimation);
		}
	}

	// UVAnimation
	auto& nextUVAnimations = nextList.uvAnimationPath;

	for (auto& loadUVAnimation : currentList.uvAnimationPath)
	{
		if (nextUVAnimations.find(loadUVAnimation) == nextUVAnimations.end())
		{
			UnloadUVAnimation(loadUVAnimation);
		}
	}

	// Material
	auto& nextMaterials = nextList.materialPaths;

	for (auto& materialPath : currentList.materialPaths)
	{
		if (nextMaterials.find(materialPath) == nextMaterials.end())
		{
			UnloadMaterial(materialPath);
		}
	}

	// NodeHierachy
	auto& nextNodeHierachy = nextList.nodeHierachyPaths;

	for (auto& nodeHierachyPath : currentList.nodeHierachyPaths)
	{
		if (nextNodeHierachy.find(nodeHierachyPath) == nextNodeHierachy.end())
		{
			UnloadNodeHierarchy(nodeHierachyPath);
		}
	}
}

void fq::game_engine::ResourceSystem::UnloadAnimation(const Path& path)
{
	mAnimations.erase(path);
}

void fq::game_engine::ResourceSystem::UnloadNodeHierarchy(const Path& path)
{
	mNodeHierarchies.erase(path);
}

void fq::game_engine::ResourceSystem::UnloadUVAnimation(const Path& path)
{
	mUVAnimations.erase(path);
}

void fq::game_engine::ResourceSystem::UnloadMaterial(const Path& path)
{
	mMaterials.erase(path);
}

