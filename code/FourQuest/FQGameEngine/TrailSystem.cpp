#include "TrailSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"

fq::game_engine::TrailSystem::TrailSystem()
	:mGameProcess(nullptr)
	, mbIsGameLoaded(false)
{}

fq::game_engine::TrailSystem::~TrailSystem()
{

}

void fq::game_engine::TrailSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle µî·Ï
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &TrailSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &TrailSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &TrailSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &TrailSystem::OnDestroyedGameObject);

	mAddComponentHandler = eventMgr->
		RegisterHandle<fq::event::AddComponent>(this, &TrailSystem::AddComponent);

	mRemoveComponentHandler = eventMgr->
		RegisterHandle<fq::event::RemoveComponent>(this, &TrailSystem::RemoveComponent);
}

void fq::game_engine::TrailSystem::Update(float dt)
{
	using namespace fq::game_module;
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<Transform, Trail>
		([dt](GameObject& object, Transform& transform, Trail& trail)
			{
				graphics::ITrailObject* trailObjectInterface = trail.GetTrailObjectInterface();

				if (trailObjectInterface != nullptr)
				{
					trailObjectInterface->SetTransform(transform.GetWorldMatrix());
					trailObjectInterface->SetFrameTime(dt);
				}
			});
}

void fq::game_engine::TrailSystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;
}

void fq::game_engine::TrailSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	if (!mbIsGameLoaded) return;

	loadTrail(event.object);
}

void fq::game_engine::TrailSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadTrail(event.object);
}

void fq::game_engine::TrailSystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::Trail>().id())
	{
		loadTrail(event.component->GetGameObject());
	}
}

void fq::game_engine::TrailSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::Trail>().id())
	{
		unloadTrail(event.component->GetGameObject());
	}
}

void fq::game_engine::TrailSystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		loadTrail(&object);
	}

	mbIsGameLoaded = true;
}

void fq::game_engine::TrailSystem::loadTrail(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Trail>())
	{
		return;
	}

	auto trail = object->GetComponent<fq::game_module::Trail>();
	fq::graphics::TrailInfo trailInfo = trail->GetTrailInfo();
	fq::graphics::ParticleMaterialInfo materialInfo = trail->GetParticleMaterialInfo();
	
	auto particleMaterial = mGameProcess->mGraphics->CreateMaterial(materialInfo);
	auto trailObjectInterface = mGameProcess->mGraphics->CreateTrailObject(object->GetComponent<fq::game_module::Transform>()->GetWorldMatrix(), trailInfo, particleMaterial);

	trail->SetTrailObjectInterface(trailObjectInterface);
	trail->SetParticleMaterial(particleMaterial);
}

void fq::game_engine::TrailSystem::unloadTrail(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Trail>())
	{
		return;
	}

	auto trail = object->GetComponent<fq::game_module::Trail>();
	auto trailObjectInterface = trail->GetTrailObjectInterface();

	if (trailObjectInterface != nullptr)
		mGameProcess->mGraphics->DeleteTrailObject(trailObjectInterface);

	if (trail->GetParticleMaterial() != nullptr)
	{
		mGameProcess->mGraphics->DeleteMaterial(trail->GetParticleMaterial());
		trail->SetParticleMaterial(nullptr);
	}
}

