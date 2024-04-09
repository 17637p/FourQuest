#include "SceneManager.h"
#include "Scene.h"

#include "EventManager.h"
#include "Event.h"

fq::game_module::SceneManager::SceneManager()
	:mCurrentScene(nullptr)
	,mEventManager(nullptr)
{
}

fq::game_module::SceneManager::~SceneManager()
{
}


void fq::game_module::SceneManager::Initialize(const std::string& startSceneName, EventManager* eventMgr)
{
	mCurrentScene = std::make_unique<Scene>();
	mEventManager = eventMgr;


	mCurrentScene->Initialize(startSceneName);

	loadScene();
	// Event CallBack
	mEventManager->FireEvent<fq::event::OnChangeScene>({startSceneName});
}

void fq::game_module::SceneManager::Finalize()
{

}

void fq::game_module::SceneManager::ChangeScene(const std::string& nextSceneName)
{



	loadScene();
	// Event CallBack
	mEventManager->FireEvent<fq::event::OnChangeScene>({ nextSceneName });
}

void fq::game_module::SceneManager::loadScene()
{

}
