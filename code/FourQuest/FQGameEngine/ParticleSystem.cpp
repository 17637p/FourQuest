#include "ParticleSystem.h"


#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"

fq::game_engine::ParticleSystem::ParticleSystem()
	:mGameProcess(nullptr)
	, mbIsGameLoaded(false)
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


void fq::game_engine::ParticleSystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;
}

void fq::game_engine::ParticleSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	if (!mbIsGameLoaded) return;

	loadParticle(event.object);
}

void fq::game_engine::ParticleSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadParticle(event.object);
}

void fq::game_engine::ParticleSystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::Particle>().id())
	{
		loadParticle(event.component->GetGameObject());
	}
}

void fq::game_engine::ParticleSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::Particle>().id())
	{
		unloadParticle(event.component->GetGameObject());
	}
}

void fq::game_engine::ParticleSystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		loadParticle(&object);
	}

	mbIsGameLoaded = true;
}

void fq::game_engine::ParticleSystem::loadParticle(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Particle>())
	{
		return;
	}

	auto particle = object->GetComponent<fq::game_module::Particle>();
	auto particleInfo = particle->GetParticleInfomation();

	auto particleObject = mGameProcess->mGraphics->CreateParticleObject(particleInfo);
	particle->SetParticleObject(particleObject);

	particleObject->SetIsEmit(true);
}

void fq::game_engine::ParticleSystem::unloadParticle(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Particle>())
	{
		return;
	}

	auto particle = object->GetComponent<fq::game_module::Particle>();
	auto particleObject = particle->GetParticleObject();

	if (particleObject != nullptr)
	{
		mGameProcess->mGraphics->DeleteParticleObject(particleObject);
		particle->SetParticleObject(nullptr);
	}
}

