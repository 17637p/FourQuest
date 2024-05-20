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
}

void fq::game_engine::RenderingSystem::OnLoadScene()
{
	// 1. Scene에 배치한 리소르를 로드 
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		loadStaticMeshRenderer(&object);
		loadSkinnedMeshRenderer(&object);
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

	// SkinnedMeshO 생성

	auto skinnedMeshObject = mGameProcess->mGraphics->CreateSkinnedMeshObject(meshInfo);
	skinnedMeshRenderer->SetSkinnedMeshObject(skinnedMeshObject);
}

void fq::game_engine::RenderingSystem::WriteAnimation(const fq::event::WriteAnimation& event)
{
	fq::common::Model model;
	fq::common::AnimationClip animationClilp;
	animationClilp.Name = event.AnimationName;
	animationClilp.FrameCount = event.animationData.size();
	animationClilp.FramePerSecond = 30.f;

	for (const auto& data : event.animationData)
	{
		fq::common::NodeClip nodeClip;
		nodeClip.NodeName = data.first;

		for (int i = 0; i < data.second.size(); i++)
		{
			fq::common::Keyframe keyFrame;
			keyFrame.TimePos = (1.f / 30.f) * i;
			data.second[i].CreateFromQuaternion(keyFrame.Rotation);
			data.second[i].CreateTranslation(keyFrame.Translation);
			data.second[i].CreateScale(keyFrame.Scale);
			nodeClip.Keyframes.push_back(keyFrame);
		}
		animationClilp.NodeClips.push_back(nodeClip);
	}

	model.Animations.push_back(animationClilp);

	mGameProcess->mGraphics->WriteModel("./" + event.AnimationName + ".model", model);
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
}

void fq::game_engine::RenderingSystem::loadAnimation(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::SkinnedMeshRenderer, fq::game_module::Animator>())
	{
		return;
	}

	// AnimatorController 로드 요청
	bool check = mGameProcess->mAnimationSystem->LoadAnimatorController(object);

	if (!check) return;

	auto animator = object->GetComponent<fq::game_module::Animator>();
	auto meshRenderer = object->GetComponent<fq::game_module::SkinnedMeshRenderer>();
	auto meshObject = meshRenderer->GetSkinnedMeshObject();

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

}

bool fq::game_engine::RenderingSystem::IsLoadedModel(const ModelPath& path)
{
	return mLoadModels.find(path) != mLoadModels.end();
}

