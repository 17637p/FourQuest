#include "DecalSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"

fq::game_engine::DecalSystem::DecalSystem()
	:mGameProcess(nullptr)
	, mbIsGameLoaded(false)
{}

fq::game_engine::DecalSystem::~DecalSystem()
{

}

void fq::game_engine::DecalSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle ���
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &DecalSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &DecalSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &DecalSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &DecalSystem::OnDestroyedGameObject);

	mAddComponentHandler = eventMgr->
		RegisterHandle<fq::event::AddComponent>(this, &DecalSystem::AddComponent);

	mRemoveComponentHandler = eventMgr->
		RegisterHandle<fq::event::RemoveComponent>(this, &DecalSystem::RemoveComponent);
}

void fq::game_engine::DecalSystem::Update(float dt)
{
	using namespace fq::game_module;
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();
	auto graphics = mGameProcess->mGraphics; // �� ���� ��ȿ���� ���´����� üũ

	scene->ViewComponents<Transform, Decal>
		([dt, graphics](GameObject& object, Transform& transform, Decal& decal)
			{
				graphics::IDecalObject* decalObjectInterface = decal.GetDecalObjectInterface();
				std::shared_ptr<graphics::IMaterial> material = decal.GetIMaterial();

				if (decalObjectInterface != nullptr && material != nullptr)
				{
					graphics->DrawDecalObject(decalObjectInterface, material, transform.GetWorldMatrix());
				}
			});
}

void fq::game_engine::DecalSystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;
}

void fq::game_engine::DecalSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	if (!mbIsGameLoaded) return;

	loadDecal(event.object);
}

void fq::game_engine::DecalSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadDecal(event.object);
}

void fq::game_engine::DecalSystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::Decal>().id())
	{
		loadDecal(event.component->GetGameObject());
	}
}

void fq::game_engine::DecalSystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::Decal>().id())
	{
		unloadDecal(event.component->GetGameObject());
	}
}

void fq::game_engine::DecalSystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		loadDecal(&object);
	}

	mbIsGameLoaded = true;
}

void fq::game_engine::DecalSystem::loadDecal(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Decal>())
	{
		return;
	}

	auto decal = object->GetComponent<fq::game_module::Decal>();
	auto decalInfo = decal->GetDecalInfo();

	auto decalObjectInterface = mGameProcess->mGraphics->CreateDecalObject(decalInfo);
	decal->SetDecalObjectInterface(decalObjectInterface);
}

void fq::game_engine::DecalSystem::unloadDecal(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Decal>())
	{
		return;
	}

	auto decal = object->GetComponent<fq::game_module::Decal>();
	auto decalObjectInterface = decal->GetDecalObjectInterface();

	if (decalObjectInterface != nullptr)
		mGameProcess->mGraphics->DeleteDecalObject(decalObjectInterface);
}

