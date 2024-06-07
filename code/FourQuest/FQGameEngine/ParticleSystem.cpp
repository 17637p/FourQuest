#include "ParticleSystem.h"


#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"

fq::game_engine::ParticleSystem::ParticleSystem()
	:mGameProcess(nullptr)
{}

fq::game_engine::ParticleSystem::~ParticleSystem()
{

}

void fq::game_engine::ParticleSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;
	
	// EventHandle µî·Ï
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &ParticleSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &ParticleSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &ParticleSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &ParticleSystem::OnDestroyedGameObject);

	mAddComponentHandler = eventMgr->
		RegisterHandle<fq::event::AddComponent>(this, &ParticleSystem::AddComponent);

	mRemoveComponentHandler = eventMgr->
		RegisterHandle<fq::event::RemoveComponent>(this, &ParticleSystem::RemoveComponent);


}

void fq::game_engine::ParticleSystem::Update(float dt)
{

}

void fq::game_engine::ParticleSystem::OnUnLoadScene()
{

}

void fq::game_engine::ParticleSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{

}

void fq::game_engine::ParticleSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{

}

void fq::game_engine::ParticleSystem::AddComponent(const fq::event::AddComponent& event)
{

}

void fq::game_engine::ParticleSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{

}

void fq::game_engine::ParticleSystem::OnLoadScene()
{

}
