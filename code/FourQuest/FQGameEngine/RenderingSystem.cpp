#include "RenderingSystem.h"

#include <filesystem>
#include <spdlog/spdlog.h>

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"
#include "AnimationSystem.h"

fq::game_engine::RenderingSystem::RenderingSystem()
	:mGameProcess(nullptr)
	, mOnAddGameObjectHandler{}
	, mOnLoadSceneHandler{}
	, mOnUnloadSceneHandler{}
	, mDestroyedGameObjectHandler{}
	, mRemoveComponentHandler{}
	, mAddComponentHandler{}
	, mbIsGameLoaded(false)
{}

fq::game_engine::RenderingSystem::~RenderingSystem()
{}

void fq::game_engine::RenderingSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle 등록
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &RenderingSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &RenderingSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &RenderingSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &RenderingSystem::OnDestroyedGameObject);

	mAddComponentHandler = eventMgr->
		RegisterHandle<fq::event::AddComponent>(this, &RenderingSystem::AddComponent);

	mRemoveComponentHandler = eventMgr->
		RegisterHandle<fq::event::RemoveComponent>(this, &RenderingSystem::RemoveComponent);

	mWriteAnimationHandler = eventMgr->
		RegisterHandle<fq::event::WriteAnimation>(this, &RenderingSystem::WriteAnimation);
}

void fq::game_engine::RenderingSystem::Update(float dt)
{
	using namespace fq::game_module;

	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<Transform, StaticMeshRenderer>
		([](GameObject& object, Transform& transform, StaticMeshRenderer& mesh)
			{
				auto meshObject = mesh.GetStaticMeshObject();
				if (meshObject)
				{
					meshObject->UpdateTransform(transform.GetWorldMatrix());
				}
			});

	scene->ViewComponents<Transform, SkinnedMeshRenderer>
		([](GameObject& object, Transform& transform, SkinnedMeshRenderer& mesh)
			{
				auto meshObject = mesh.GetSkinnedMeshObject();
				if (meshObject)
				{
					meshObject->UpdateTransform(transform.GetWorldMatrix());
				}
			});

	scene->ViewComponents<Transform, Terrain>
		([this](GameObject& object, Transform& transform, Terrain& mesh)
			{
				auto meshObject = mesh.GetTerrainMeshObject();
				if (meshObject)
				{
					meshObject->SetTransform(mPlaneMatrix * transform.GetWorldMatrix());
				}
			});
}

void fq::game_engine::RenderingSystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	// 1. Scene에 배치한 리소르를 로드 
	for (auto& object : scene->GetObjectView(true))
	{
		loadStaticMeshRenderer(&object);
		loadSkinnedMeshRenderer(&object);
		loadTerrain(&object);
	}

	// 2. 애니메이션을 연결
	for (auto& object : scene->GetObjectView(true))
	{
		loadAnimation(&object);
	}

	// 2. PrefabInstance를 로드



	mbIsGameLoaded = true;
}

void fq::game_engine::RenderingSystem::OnUnLoadScene()
{
	// 1. Model Unload
	unloadAllModel();

	mbIsGameLoaded = false;
}

void fq::game_engine::RenderingSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	// 씬이 로드된 시점에서만 처리합니다
	if (!mbIsGameLoaded)
	{
		return;
	}
	auto gameObject = event.object;

	// 오브젝트에 필요한 리소스를 로드합니다

	// 1. StaticMesh 
	loadStaticMeshRenderer(gameObject);

	// 2. SkinnedMesh
	loadSkinnedMeshRenderer(gameObject);

	// 3. Animation
	loadAnimation(gameObject);

	// 4. Terrain
	loadTerrain(gameObject);
}

void fq::game_engine::RenderingSystem::loadSkinnedMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::SkinnedMeshRenderer>())
	{
		return;
	}

	auto skinnedMeshRenderer = object->GetComponent<fq::game_module::SkinnedMeshRenderer>();
	auto meshInfo = skinnedMeshRenderer->GetMeshObjectInfomation();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	if (!std::filesystem::exists(meshInfo.ModelPath))
	{
		SPDLOG_WARN("\"{}\" does not exist", meshInfo.ModelPath);
	}
	else
	{
		LoadModel(meshInfo.ModelPath);
	}
	meshInfo.Transform = transform->GetLocalMatrix();

	// SkinnedMesh 생성
	auto skinnedMeshObject = mGameProcess->mGraphics->CreateSkinnedMeshObject(meshInfo);
	skinnedMeshRenderer->SetSkinnedMeshObject(skinnedMeshObject);

	if (skinnedMeshObject)
		skinnedMeshObject->SetOutlineColor(skinnedMeshRenderer->GetOutlineColor());
}

void fq::game_engine::RenderingSystem::WriteAnimation(const fq::event::WriteAnimation& event)
{
	static std::vector<fq::common::AnimationClip> mAnimationClips;

	fq::common::AnimationClip animationClilp;
	animationClilp.Name = event.animationName;
	animationClilp.FrameCount = event.animationData.begin()->second.size();
	animationClilp.FramePerSecond = (1.f / 30.f);
	animationClilp.Duration = (1.f / 30.f);

	for (const auto& data : event.animationData)
	{
		fq::common::NodeClip nodeClip;
		nodeClip.NodeName = data.first;

		for (int i = 0; i < data.second.size(); i++)
		{
			DirectX::SimpleMath::Matrix curMatrix = data.second[i];
			fq::common::Keyframe keyFrame;
			curMatrix.Decompose(keyFrame.Scale, keyFrame.Rotation, keyFrame.Translation);
			keyFrame.TimePos = (1.f / 30.f) * i;
			nodeClip.Keyframes.push_back(keyFrame);
		}
		animationClilp.NodeClips.push_back(nodeClip);
	}

	mAnimationClips.push_back(animationClilp);

	if (mAnimationClips.size() >= event.animationSize)
	{
		fq::common::Model model;
		model.Animations = mAnimationClips;
		mGameProcess->mGraphics->WriteModel("./" + event.animationName + ".model", model);
		mAnimationClips.clear();
	}
}

void fq::game_engine::RenderingSystem::loadStaticMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::StaticMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::StaticMeshRenderer>();
	auto meshInfo = staticMeshRenderer->GetMeshObjectInfomation();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	// Model 생성
	if (!std::filesystem::exists(meshInfo.ModelPath))
	{
		SPDLOG_WARN("[RenderingSystem] Model Path \"{}\" does not exist", meshInfo.ModelPath);
	}
	else
	{
		LoadModel(meshInfo.ModelPath);
	}
	meshInfo.Transform = transform->GetLocalMatrix();

	// StaticMesh 생성
	auto staticMeshObject = mGameProcess->mGraphics->CreateStaticMeshObject(meshInfo);
	staticMeshRenderer->SetStaticMeshObject(staticMeshObject);
	staticMeshObject->SetOutlineColor(staticMeshRenderer->GetOutlineColor());
}

void fq::game_engine::RenderingSystem::loadAnimation(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<game_module::Animator>())
	{
		return;
	}

	// AnimatorController 로드 요청
	bool check = mGameProcess->mAnimationSystem->LoadAnimatorController(object);

	if (!check) return;

	auto animator = object->GetComponent<fq::game_module::Animator>();
	auto& animatorMeshs = animator->GetSkinnedMeshs();

	// 자식 계층의 메쉬들을 연결합니다.
	for (auto& child : object->GetChildren())
	{
		if (!child->HasComponent<game_module::SkinnedMeshRenderer>()) continue;

		auto meshRenderer = child->GetComponent<fq::game_module::SkinnedMeshRenderer>();
		auto meshObject = meshRenderer->GetSkinnedMeshObject();
		if (!meshObject) return;

		animatorMeshs.push_back(meshRenderer);

		const auto& stateMap = animator->GetController().GetStateMap();

		for (const auto& [id, state] : stateMap)
		{
			fq::graphics::AnimationInfo info;

			info.ModelPath = state.GetModelPath();
			info.AnimationName = state.GetAnimationName();
			info.AnimationKey = state.GetAnimationKey();

			if (info.ModelPath.empty()
				|| info.AnimationName.empty()
				|| info.AnimationKey.empty())
				continue;

			LoadModel(info.ModelPath);
			mGameProcess->mGraphics->AddAnimation(meshObject, info);
		}
	}
}


void fq::game_engine::RenderingSystem::LoadModel(const ModelPath& path)
{
	auto iter = mLoadModels.find(path);

	if (iter == mLoadModels.end())
	{
		std::filesystem::path texturePath = path;
		texturePath = texturePath.remove_filename();

		mGameProcess->mGraphics->CreateModel(path, texturePath);
		mLoadModels.insert(path);
	}
}

void fq::game_engine::RenderingSystem::unloadAllModel()
{
	for (auto& modelPath : mLoadModels)
	{
		mGameProcess->mGraphics->DeleteModel(modelPath);
	}

	mLoadModels.clear();
}

void fq::game_engine::RenderingSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadStaticMeshRenderer(event.object);
	unloadSkinnedMeshRenderer(event.object);
	unloadTerrain(event.object);
}

void fq::game_engine::RenderingSystem::unloadStaticMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::StaticMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::StaticMeshRenderer>();
	auto staticMesh = staticMeshRenderer->GetStaticMeshObject();
	mGameProcess->mGraphics->DeleteStaticMeshObject(staticMesh);
	staticMeshRenderer->SetStaticMeshObject(nullptr);
}


void fq::game_engine::RenderingSystem::unloadSkinnedMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::SkinnedMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::SkinnedMeshRenderer>();
	auto staticMesh = staticMeshRenderer->GetSkinnedMeshObject();
	mGameProcess->mGraphics->DeleteSkinnedMeshObject(staticMesh);
	staticMeshRenderer->SetSkinnedMeshObject(nullptr);
}

void fq::game_engine::RenderingSystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::StaticMeshRenderer>().id())
	{
		loadStaticMeshRenderer(event.component->GetGameObject());
	}

	if (event.id == entt::resolve<fq::game_module::SkinnedMeshRenderer>().id())
	{
		loadSkinnedMeshRenderer(event.component->GetGameObject());
	}

	if (event.id == entt::resolve<fq::game_module::Terrain>().id())
	{
		loadTerrain(event.component->GetGameObject());
	}
}

void fq::game_engine::RenderingSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::StaticMeshRenderer>().id())
	{
		unloadStaticMeshRenderer(event.component->GetGameObject());
	}

	if (event.id == entt::resolve<fq::game_module::SkinnedMeshRenderer>().id())
	{
		unloadSkinnedMeshRenderer(event.component->GetGameObject());
	}

	if (event.id == entt::resolve<fq::game_module::Terrain>().id())
	{
		unloadTerrain(event.component->GetGameObject());
	}
}

bool fq::game_engine::RenderingSystem::IsLoadedModel(const ModelPath& path)
{
	return mLoadModels.find(path) != mLoadModels.end();
}

void fq::game_engine::RenderingSystem::loadTerrain(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Terrain>())
	{
		return;
	}

	auto terrain = object->GetComponent<fq::game_module::Terrain>();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	std::string terrainPath = "./resource/internal/terrain/Plane.model";

	// Model 생성
	LoadModel(terrainPath);
	const fq::common::Model& modelData = mGameProcess->mGraphics->GetModel(terrainPath);
	auto mesh = modelData.Meshes[1];

	fq::graphics::MeshObjectInfo meshInfo;
	meshInfo.ModelPath = terrainPath;
	meshInfo.MeshName = mesh.second.Name;
	meshInfo.Transform = mesh.first.ToParentMatrix * transform->GetWorldMatrix();

	mPlaneMatrix = mesh.first.ToParentMatrix;

	fq::graphics::ITerrainMeshObject* iTerrainMeshObject = mGameProcess->mGraphics->CreateTerrainMeshObject(meshInfo);
	terrain->SetTerrainMeshObject(iTerrainMeshObject);

	fq::graphics::TerrainMaterialInfo info;
	info.AlPhaFileName = terrain->GetAlphaMap();
	info.Layers = terrain->GetTerrainLayers();
	info.Width = terrain->GetWidth();
	info.Height = terrain->GetHeight();
	info.HeightScale = terrain->GetHeightScale();
	info.HeightFileName = terrain->GetHeightMap();

	mGameProcess->mGraphics->SetTerrainMeshObject(iTerrainMeshObject, info);
}

void fq::game_engine::RenderingSystem::unloadTerrain(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Terrain>())
	{
		return;
	}

	auto terrain = object->GetComponent<fq::game_module::Terrain>();
	auto terrainMeshObject = terrain->GetTerrainMeshObject();
	mGameProcess->mGraphics->DeleteTerrainMeshObject(terrainMeshObject);
	terrain->SetTerrainMeshObject(nullptr);
}