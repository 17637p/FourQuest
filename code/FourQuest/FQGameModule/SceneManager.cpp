#include "SceneManager.h"
#include "Scene.h"

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <spdlog/fmt/chrono.h>

#include "../FQCommon/FQPath.h"

#include "PrefabManager.h"
#include "InputManager.h"
#include "EventManager.h"
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
	, PrefabManager* prefabMgr)
{
	mCurrentScene = std::make_unique<Scene>();
	mEventManager = eventMgr;
	mPrefabManager = prefabMgr;

	mCurrentScene->Initialize(startSceneName, eventMgr, inputMgr, prefabMgr);

	mRequestExitGameHadler =
		mEventManager->RegisterHandle<fq::event::RequestExitGame>(this, &SceneManager::RequestExitGame);

	mRequestChangeSceneHandler =
		mEventManager->RegisterHandle<fq::event::RequestChangeScene>(this, &SceneManager::RequestChangeScene);
}

void fq::game_module::SceneManager::Finalize()
{
	mEventManager->RemoveHandle(mRequestChangeSceneHandler);
	mEventManager->RemoveHandle(mRequestExitGameHadler);

}

void fq::game_module::SceneManager::ChangeScene(const std::string& nextSceneName)
{
	UnloadScene();

	mCurrentScene->mSceneName = nextSceneName;

	LoadScene();

	if (mbIsInvokeStartScene)
	{
		StartScene();
	}
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
	for (auto& object : mCurrentScene->GetObjectView())
	{
		object.OnStart();
	}

	//Event CaallBack
	mEventManager->FireEvent<fq::event::StartScene>({});
}

void fq::game_module::SceneManager::PostUpdate()
{
	// ������ ó�� 
	if (!mNextSceneName.empty())
	{
		ChangeScene(mNextSceneName);

		mNextSceneName.clear();
	}
}

void fq::game_module::SceneManager::LoadScene()
{
	spdlog::stopwatch sw;

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

	// Event CallBack
	mEventManager->FireEvent<fq::event::OnLoadScene>({ mCurrentScene->GetSceneName() });

	spdlog::trace("[SceneManager] Load \"{}\" Scene [{}s]", mCurrentScene->GetSceneName(), sw);
}

void fq::game_module::SceneManager::UnloadScene()
{
	mEventManager->FireEvent<fq::event::OnUnloadScene>({});
	mPrefabManager->UnloadPrefabResource();
	mCurrentScene->DestroyAll();
	mCurrentScene->CleanUp();
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


