#include "SceneManager.h"
#include "Scene.h"

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <spdlog/fmt/chrono.h>

#include "../FQCommon/FQPath.h"

#include "PrefabManager.h"
#include "InputManager.h"
#include "EventManager.h"
#include "TimeManager.h"
#include "GameObject.h"

fq::game_module::SceneManager::SceneManager()
	:mCurrentScene(nullptr)
	, mPrefabManager(nullptr)
	, mEventManager(nullptr)
	, mbIsEnd(false)
	, mNextSceneName{}
	, mRequestChangeSceneHandler{}
	, mRequestExitGameHadler{}
	, mbIsInvokeStartScene(false)
{
}

fq::game_module::SceneManager::~SceneManager()
{
}

void fq::game_module::SceneManager::Initialize(const std::string& startSceneName
	, EventManager* eventMgr
	, InputManager* inputMgr
	, PrefabManager* prefabMgr
	, ScreenManager* screenMgr
	, TimeManager* timeMgr
	, bool isInvokeStartScene)
{
	mCurrentScene = std::make_unique<Scene>();
	mEventManager = eventMgr;
	mPrefabManager = prefabMgr;
	mTimeManager = timeMgr;

	mCurrentScene->Initialize(startSceneName, eventMgr, inputMgr, prefabMgr, screenMgr, timeMgr);

	mRequestExitGameHadler =
		mEventManager->RegisterHandle<fq::event::RequestExitGame>(this, &SceneManager::RequestExitGame);

	mRequestChangeSceneHandler =
		mEventManager->RegisterHandle<fq::event::RequestChangeScene>(this, &SceneManager::RequestChangeScene);

	mbIsInvokeStartScene = isInvokeStartScene;
}

void fq::game_module::SceneManager::Finalize()
{
	mEventManager->RemoveHandle(mRequestChangeSceneHandler);
	mEventManager->RemoveHandle(mRequestExitGameHadler);
}

void fq::game_module::SceneManager::ChangeScene()
{
	UnloadScene();
	LoadScene();
	StartScene();
}

void fq::game_module::SceneManager::Update(float dt)
{
	for (auto& object : mCurrentScene->GetObjectView())
	{
		object.OnUpdate(dt);
	}
}

void fq::game_module::SceneManager::LateUpdate(float dt)
{
	for (auto& object : mCurrentScene->GetObjectView())
	{
		object.OnLateUpdate(dt);
	}
}

void fq::game_module::SceneManager::FixedUpdate(float dt)
{
	for (auto& object : mCurrentScene->GetObjectView())
	{
		object.OnFixedUpdate(dt);
	}
}

void fq::game_module::SceneManager::StartScene()
{
	if (!mbIsInvokeStartScene) return;

	mCurrentScene->mIsStartScene = true;

	for (auto& object : mCurrentScene->GetObjectView())
	{
		object.OnAwake();
	}

	for (auto& object : mCurrentScene->GetObjectView())
	{
		object.OnStart();
	}

	//Event CaallBack
	mEventManager->FireEvent<fq::event::StartScene>({});
}

void fq::game_module::SceneManager::PostUpdate()
{
	// ���ӿ�����Ʈ �߰� ó�� 
	mCurrentScene->processPedingObject();
}

void fq::game_module::SceneManager::LoadScene()
{
	auto scenePath = fq::path::GetScenePath();

	scenePath /= mCurrentScene->GetSceneName();
		assert(std::filesystem::exists(scenePath));

	// Prefab Load
	auto prefabPath = scenePath / "prefab";
	assert(std::filesystem::exists(prefabPath));
 
	auto prefabList = fq::path::GetFileList(prefabPath);
	for (const auto& prefabPath : prefabList)
	{
		auto prefab = mPrefabManager->LoadPrefab(prefabPath);
		assert(!prefab.empty());

		// ���� ���������� �߰��ϸ� �Ʒ� ������ �ȿ��� ���� �߰��մϴ� 
		mCurrentScene->AddGameObject(prefab[0]);
	}

	// PrefabResource Load
	mPrefabManager->LoadPrefabResource(mCurrentScene.get());
}

void fq::game_module::SceneManager::UnloadScene()
{
	mTimeManager->SetTimeScale(1);
	mEventManager->FireEvent<fq::event::OnUnloadScene>({});
	mCurrentScene->DestroyAll();
	mCurrentScene->CleanUp();
	mPrefabManager->UnloadPrefabResource();
	mCurrentScene->mIsStartScene = false;
	mCurrentScene->mSceneName = mNextSceneName;
	mNextSceneName.clear();
}

void fq::game_module::SceneManager::RequestChangeScene(fq::event::RequestChangeScene event)
{
	// ������ ��û�� ���� ���� ��û�� ó���մϴ�
	if (mNextSceneName.empty())
	{
		mNextSceneName = event.sceneName;
		mbIsInvokeStartScene = event.bIsInvokeStartScene;
	}
}

void fq::game_module::SceneManager::RequestExitGame(fq::event::RequestExitGame event)
{
	mbIsEnd = true;
}

bool fq::game_module::SceneManager::IsChangeScene() const
{
	return !mNextSceneName.empty();
}

