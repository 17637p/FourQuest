#include "PrefabObjectSystem.h"

#include "GameProcess.h"

fq::game_engine::PrefabObjectSystem::PrefabObjectSystem()
	:mEventManager(nullptr)
	, mObjectManager(nullptr)
{}

fq::game_engine::PrefabObjectSystem::~PrefabObjectSystem()
{}

void fq::game_engine::PrefabObjectSystem::Initialize(GameProcess* game)
{
	mEventManager = game->mEventManager.get();
	mObjectManager = game->mObjectManager.get();

	mOnLoadSceneHandler = mEventManager->
		RegisterHandle<fq::event::OnLoadScene>(this, &PrefabObjectSystem::OnLoadScene);
	mOnUnloadSceneHandler = mEventManager->
		RegisterHandle<fq::event::OnUnloadScene>(this, &PrefabObjectSystem::OnUnloadScene);
}

void fq::game_engine::PrefabObjectSystem::OnLoadScene(const fq::event::OnLoadScene& event)
{}

void fq::game_engine::PrefabObjectSystem::OnUnloadScene()
{}

void fq::game_engine::PrefabObjectSystem::InstantiatePrefab(fq::event::InstantiatePrefab event)
{
	auto path = event.instantiatePrefabObject.GetPrefabPath();
	event.instance =  mObjectManager->LoadPrefab(path);
}
