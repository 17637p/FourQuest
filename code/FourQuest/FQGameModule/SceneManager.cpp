#include "SceneManager.h"
#include "Scene.h"

#include "../FQCommon/FQPath.h"

#include "ObjectManager.h"
#include "InputManager.h"
#include "EventManager.h"
#include "GameObject.h"

fq::game_module::SceneManager::SceneManager()
	:mCurrentScene(nullptr)
	, mObjectManager(nullptr)
	, mEventManager(nullptr)
	, mbIsEnd(false)
	, mNextSceneName{}
	, mRequestChangeSceneHandler{}
	, mRequestExitGameHadler{}
{
}

fq::game_module::SceneManager::~SceneManager()
{
}

void fq::game_module::SceneManager::Initialize(const std::string& startSceneName, EventManager* eventMgr, InputManager* inputMgr)
{
	mCurrentScene = std::make_unique<Scene>();
	mEventManager = eventMgr;
	mNextSceneName = startSceneName;

	mCurrentScene->Initialize(startSceneName, eventMgr, inputMgr);

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
	mEventManager->FireEvent<fq::event::OnUnloadScene>({});

	mCurrentScene->mSceneName = nextSceneName;

	LoadScene();
	// Event CallBack
	mEventManager->FireEvent<fq::event::OnLoadScene>({ nextSceneName });
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
}

void fq::game_module::SceneManager::PostUpdate()
{
	// 삭제 예정인 오브젝트 삭제처리 
	mCurrentScene->CleanUp();

	// 씬변경 처리 
	if (mCurrentScene->GetSceneName() != mNextSceneName)
	{
		ChangeScene(mNextSceneName);
	}
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
		auto prefab = mObjectManager->LoadPrefab(prefabPath);
		assert(!prefab.empty());

		mCurrentScene->AddGameObject(prefab[0]);
	}

}

void fq::game_module::SceneManager::UnloadScene()
{
	mCurrentScene->CleanUp();

	mCurrentScene->mObjects.clear();
}

void fq::game_module::SceneManager::RequestChangeScene(fq::event::RequestChangeScene event)
{
	mNextSceneName = event.sceneName;
}

void fq::game_module::SceneManager::RequestExitGame(fq::event::RequestExitGame event)
{
	mbIsEnd = true;
}

