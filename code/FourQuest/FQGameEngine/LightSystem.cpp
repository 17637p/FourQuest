#include "LightSystem.h"

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

fq::game_engine::LightSystem::LightSystem()
	:mGameProcess(nullptr)
	, mbIsGameLoaded(false)
{}

fq::game_engine::LightSystem::~LightSystem()
{}

void fq::game_engine::LightSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;

	// EventHandle 등록
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &LightSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &LightSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = eventMgr->
		RegisterHandle<fq::event::AddGameObject>(this, &LightSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = eventMgr->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &LightSystem::OnDestroyedGameObject);

	mSetLightInfoHandler = eventMgr->
		RegisterHandle<fq::event::SetLightInfo>(this, &LightSystem::SetLightInfo);
}

void fq::game_engine::LightSystem::OnLoadScene(const fq::event::OnLoadScene event)
{
	// 1. Scene에 배치한 리소르를 로드 
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		addLight(&object);
	}

	mbIsGameLoaded = true;
}

void fq::game_engine::LightSystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;
}

void fq::game_engine::LightSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	// 씬이 로드된 시점에서만 처리합니다
	if (!mbIsGameLoaded)
	{
		return;
	}

	// Light 추가 
	addLight(event.object);
}

void fq::game_engine::LightSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	deleteLight(event.object);
}

void fq::game_engine::LightSystem::SetLightInfo(const fq::event::SetLightInfo& event)
{
	auto id = event.light->GetGameObject()->GetID();

	// 라이트를 재생성
	if (event.bIsChangedType)
	{
		mGameProcess->mGraphics->DeleteLight(id);
		mGameProcess->mGraphics->AddLight(id, event.light->GetLightInfomation());
	}
	else
	{

	}

}

void fq::game_engine::LightSystem::addLight(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Light>())
	{
		return;
	}

	auto light = object->GetComponent<fq::game_module::Light>();
	auto transform = object->GetComponent<fq::game_module::Transform>();
	updateLight(*light, *transform);

	auto lightInfo = light->GetLightInfomation();
	mGameProcess->mGraphics->AddLight(object->GetID(), lightInfo);
}

void fq::game_engine::LightSystem::updateLight(fq::game_module::Light& light, fq::game_module::Transform& transform)
{
	auto lightInfo = light.GetLightInfomation();

	lightInfo.position = transform.GetWorldPosition();
	lightInfo.direction = transform.GetWorldMatrix().Forward();

	light.SetLightInfomation(lightInfo);
}

void fq::game_engine::LightSystem::deleteLight(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::Light>())
	{
		return;
	}

	mGameProcess->mGraphics->DeleteLight(object->GetID());
}

