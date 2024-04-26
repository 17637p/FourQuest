#include "PhysicsSystem.h"

#include "../FQphysics/IFQPhysics.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

fq::game_engine::PhysicsSystem::PhysicsSystem()
	:mGameProcess(nullptr)
{}

fq::game_engine::PhysicsSystem::~PhysicsSystem()
{}

void fq::game_engine::PhysicsSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;

	// EventHandle µî·Ï
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &PhysicsSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &PhysicsSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &PhysicsSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &PhysicsSystem::OnDestroyedGameObject);
}

void fq::game_engine::PhysicsSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{


}

void fq::game_engine::PhysicsSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{

}

void fq::game_engine::PhysicsSystem::OnUnLoadScene()
{

}

void fq::game_engine::PhysicsSystem::OnLoadScene()
{

}
