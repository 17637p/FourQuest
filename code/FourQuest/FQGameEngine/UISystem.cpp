#include "UISystem.h"

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Transform.h"
#include "GameProcess.h"

//#include "../FQGameModule/Transform.h"

fq::game_engine::UISystem::UISystem()
	:mGameProcess(nullptr)
	, mbIsGameLoaded(false)
	, mViewporWidth(0)
	, mViewportHeight(0)
	, mScreenHeight(0)
	, mScreenWidth(0)
{}

fq::game_engine::UISystem::~UISystem()
{

}

void fq::game_engine::UISystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle µî·Ï
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &UISystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &UISystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &UISystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &UISystem::OnDestroyedGameObject);

	mAddComponentHandler = eventMgr->
		RegisterHandle<fq::event::AddComponent>(this, &UISystem::AddComponent);

	mRemoveComponentHandler = eventMgr->
		RegisterHandle<fq::event::RemoveComponent>(this, &UISystem::RemoveComponent);

	mSetUIInfomationsHandler = eventMgr->
		RegisterHandle<fq::event::SetUIInfomations>(this, &UISystem::SetUIInfomations);

	mSetScreenSizeHandler = eventMgr->
		RegisterHandle<fq::event::SetScreenSize>([this](fq::event::SetScreenSize event)
			{
				mScreenWidth = event.width;
				mScreenHeight = event.height;
			});

	mSetScreenSizeHandler = eventMgr->
		RegisterHandle<fq::event::SetViewportSize>([this](fq::event::SetViewportSize event)
			{
				mViewporWidth = event.width;
				mViewportHeight = event.height;
			});
}

void fq::game_engine::UISystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		loadImageUI(&object);
	}

	mbIsGameLoaded = true;
}

void fq::game_engine::UISystem::OnUnLoadScene()
{
	mbIsGameLoaded = false;
}

void fq::game_engine::UISystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	if (!mbIsGameLoaded) return;

	loadImageUI(event.object);
}

void fq::game_engine::UISystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadImageUI(event.object);
}

void fq::game_engine::UISystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::ImageUI>().id())
	{
		loadImageUI(event.component->GetGameObject());
	}
}

void fq::game_engine::UISystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::ImageUI>().id())
	{
		unloadImageUI(event.component->GetGameObject());
	}
}

void fq::game_engine::UISystem::loadImageUI(game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::ImageUI>())
	{
		return;
	}

	auto imageUI = object->GetComponent<game_module::ImageUI>();
	auto imageInfomations = imageUI->GetUIInfomations();
	auto& imageObjects = imageUI->GetImageObjects();

	for (const auto& imageInfomation : imageInfomations)
	{
		if (!std::filesystem::exists(imageInfomation.ImagePath))
		{
			spdlog::warn("[UISystem] {} Load failed", imageInfomation.ImagePath );
			imageObjects.push_back(nullptr);
		}
		else
		{
			auto imageObject = mGameProcess->mGraphics->CreateImageObject(imageInfomation);
			imageObjects.push_back(imageObject);
		}
	}
}

void fq::game_engine::UISystem::unloadImageUI(game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::ImageUI>())
	{
		return;
	}

	auto imageUI = object->GetComponent<game_module::ImageUI>();

	auto& imageObjects = imageUI->GetImageObjects();

	for (auto imageObject : imageObjects)
	{
		if (imageObject != nullptr)
			mGameProcess->mGraphics->DeleteImageObject(imageObject);
	}
	imageObjects.clear();
}

void fq::game_engine::UISystem::SetUIInfomations(const fq::event::SetUIInfomations& event)
{
	unloadImageUI(event.object);
	loadImageUI(event.object);
}

void fq::game_engine::UISystem::Update()
{
	auto componentView = mGameProcess->mSceneManager->GetCurrentScene()->GetComponentView<game_module::ImageUI>();

	float distMin = FLT_MAX;

	for (auto& imageUI : componentView)
	{
		game_module::Transform* transform = imageUI.GetComponent<game_module::Transform>();
		imageUI.GetComponent<game_module::ImageUI>()->OnUpdate(0);
		//imageUI.GetComponent<game_module::ImageUI>()->SetUIPosition(0, transform->GetWorldPosition().x, transform->GetWorldPosition().y);
	}
}
