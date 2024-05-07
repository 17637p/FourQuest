#include "SoundSystem.h"

#include "GameProcess.h"

void fq::game_engine::SoundSystem::Initialize(fq::game_engine::GameProcess* game)
{
	mEventManager = game->mEventManager.get();
	mSoundManager = game->mSoundManager.get();
	mScene = game->mSceneManager->GetCurrentScene();

	mOnLoadSceneHandler = mEventManager->
		RegisterHandle<fq::event::OnLoadScene>(this, &SoundSystem::OnLoadScene);

	mOnUnloadSceneHandler = mEventManager->
		RegisterHandle<fq::event::OnUnloadScene>(this, &SoundSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &SoundSystem::OnAddGameObject);

	mAddComponentHandler = mEventManager->
		RegisterHandle<fq::event::AddComponent>(this, &SoundSystem::AddComponent);
}

void fq::game_engine::SoundSystem::OnLoadScene(const fq::event::OnLoadScene event)
{

	mbIsGameLoaded = true;
}

void fq::game_engine::SoundSystem::OnUnLoadScene()
{
	mSoundManager->UnloadAllSound();

	mbIsGameLoaded = false;
}

void fq::game_engine::SoundSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{

}

void fq::game_engine::SoundSystem::AddComponent(const fq::event::AddComponent& event)
{

}
