#include "UISystem.h"

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/SpriteAnimationUI.h"
#include "../FQGameModule/VideoUI.h"
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

	mSetTextInformationHandler = eventMgr->
		RegisterHandle<fq::event::SetTextInformation>(this, &UISystem::SetTextInformation);

	mSetSpriteInformationHandler = eventMgr->
		RegisterHandle<fq::event::SetSpriteAnimationInformation>(this, &UISystem::SetSpriteAnimationInformation);

	mSetVideoInformationHandler = eventMgr->
		RegisterHandle<fq::event::SetVideoInformation>(this, &UISystem::SetVideoInformation);

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

	mGameProcess->mGraphics->AddFont(L"resource/internal/font/ÇÑÄÄ ¸»¶û¸»¶û.ttf");
	mGameProcess->mGraphics->AddFont(L"resource/internal/font/DungGeunMo.ttf");
	mGameProcess->mGraphics->AddFont(L"resource/internal/font/´øÆÄ ¿¬´ÜµÈ Ä®³¯.ttf");
}

void fq::game_engine::UISystem::Finalize()
{
	mGameProcess->mGraphics->DeleteFont(L"resource/internal/font/ÇÑÄÄ ¸»¶û¸»¶û.ttf");
	mGameProcess->mGraphics->DeleteFont(L"resource/internal/font/DungGeunMo.ttf");
	mGameProcess->mGraphics->DeleteFont(L"resource/internal/font/´øÆÄ ¿¬´ÜµÈ Ä®³¯.ttf");
}

void fq::game_engine::UISystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		LoadImageUI(&object);
		LoadTextUI(&object);
		LoadSpriteAnimationUI(&object);
		LoadVideoUI(&object);
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

	LoadImageUI(event.object);
	LoadTextUI(event.object);
	LoadSpriteAnimationUI(event.object);
	LoadVideoUI(event.object);
}

void fq::game_engine::UISystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	UnloadImageUI(event.object);
	UnloadTextUI(event.object);
	UnloadSpriteAnimationUI(event.object);
	UnloadVideoUI(event.object);
}

void fq::game_engine::UISystem::AddComponent(const fq::event::AddComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::ImageUI>().id())
	{
		LoadImageUI(event.component->GetGameObject());
	}
	if (event.id == entt::resolve<fq::game_module::TextUI>().id())
	{
		LoadTextUI(event.component->GetGameObject());
	}
	if (event.id == entt::resolve<fq::game_module::SpriteAnimationUI>().id())
	{
		LoadSpriteAnimationUI(event.component->GetGameObject());
	}
	if (event.id == entt::resolve<fq::game_module::VideoUI>().id())
	{
		LoadVideoUI(event.component->GetGameObject());
	}
}

void fq::game_engine::UISystem::RemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == entt::resolve<fq::game_module::ImageUI>().id())
	{
		UnloadImageUI(event.component->GetGameObject());
	}
	if (event.id == entt::resolve<fq::game_module::TextUI>().id())
	{
		UnloadTextUI(event.component->GetGameObject());
	}
	if (event.id == entt::resolve<fq::game_module::SpriteAnimationUI>().id())
	{
		UnloadSpriteAnimationUI(event.component->GetGameObject());
	}
	if (event.id == entt::resolve<fq::game_module::VideoUI>().id())
	{
		UnloadVideoUI(event.component->GetGameObject());
	}
}

void fq::game_engine::UISystem::LoadImageUI(game_module::GameObject* object)
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
			spdlog::warn("[UISystem] {} Load failed", imageInfomation.ImagePath);
			imageObjects.push_back(nullptr);
		}
		else
		{
			auto imageObject = mGameProcess->mGraphics->CreateImageObject(imageInfomation);
			imageObjects.push_back(imageObject);
		}
	}
}

void fq::game_engine::UISystem::UnloadImageUI(game_module::GameObject* object)
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
	UnloadImageUI(event.object);
	LoadImageUI(event.object);
}

void fq::game_engine::UISystem::Update(float dt)
{
	auto componentView = mGameProcess->mSceneManager->GetCurrentScene()->GetComponentView<game_module::ImageUI>();

	for (auto& imageUI : componentView)
	{
		imageUI.GetComponent<game_module::ImageUI>()->OnUpdate(0);
	}

	auto textComponentView = mGameProcess->mSceneManager->GetCurrentScene()->GetComponentView<game_module::TextUI>();

	for (auto& textUI : textComponentView)
	{
		textUI.GetComponent<game_module::TextUI>()->OnUpdate(0);
	}

	auto spriteAnimationView = mGameProcess->mSceneManager->GetCurrentScene()->GetComponentView<game_module::SpriteAnimationUI>();

	for (auto& spriteAnimationUI : spriteAnimationView)
	{
		spriteAnimationUI.GetComponent<game_module::SpriteAnimationUI>()->OnUpdate(dt);
	}
}

void fq::game_engine::UISystem::LoadTextUI(game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::TextUI>())
	{
		return;
	}

	game_module::TextUI* textUI = object->GetComponent<game_module::TextUI>();
	graphics::TextInfo textInfo = textUI->GetTextInfo();

	graphics::ITextObject* textObject = mGameProcess->mGraphics->CreateText(textInfo);
	textUI->SetTextObject(textObject);
}

void fq::game_engine::UISystem::UnloadTextUI(game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::TextUI>())	
	{
		return;
	}

	game_module::TextUI* textUI = object->GetComponent<game_module::TextUI>();
	graphics::ITextObject* textObject = textUI->GetTextObject();

	if (textObject)
	{
		mGameProcess->mGraphics->DeleteText(textObject);
		textUI->SetTextObject(nullptr);
	}
}

void fq::game_engine::UISystem::SetTextInformation(const fq::event::SetTextInformation& event)
{
	UnloadTextUI(event.object);
	LoadTextUI(event.object);
}

void fq::game_engine::UISystem::LoadSpriteAnimationUI(game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::SpriteAnimationUI>())
	{
		return;
	}

	game_module::SpriteAnimationUI* spriteAnimationUI = object->GetComponent<game_module::SpriteAnimationUI>();
	graphics::SpriteInfo spriteInfo = spriteAnimationUI->GetSpriteInfo();

	if (!std::filesystem::exists(spriteInfo.ImagePath))
	{
		spdlog::warn("[UISystem] {} Load failed", spriteInfo.ImagePath);
	}
	else
	{
		graphics::ISpriteAnimationObject* spriteAnimationObject = mGameProcess->mGraphics->CreateSpriteAnimation(spriteInfo);
		spriteAnimationUI->SetSpriteAnimationObject(spriteAnimationObject);
	}
}

void fq::game_engine::UISystem::UnloadSpriteAnimationUI(game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::SpriteAnimationUI>())
	{
		return;
	}

	game_module::SpriteAnimationUI* spriteAnimationUI = object->GetComponent<game_module::SpriteAnimationUI>();

	graphics::ISpriteAnimationObject* spriteAnimationObject = spriteAnimationUI->GetSpriteAnimationObject();

	if (spriteAnimationObject)
	{
		mGameProcess->mGraphics->DeleteSpriteAnimation(spriteAnimationObject);
		spriteAnimationUI->SetSpriteAnimationObject(nullptr);
	}
}

void fq::game_engine::UISystem::SetSpriteAnimationInformation(const fq::event::SetSpriteAnimationInformation& event)
{
	UnloadSpriteAnimationUI(event.object);
	LoadSpriteAnimationUI(event.object);
}

void fq::game_engine::UISystem::SetVideoInformation(const fq::event::SetVideoInformation& event)
{
	UnloadVideoUI(event.object);
	LoadVideoUI(event.object);
}

void fq::game_engine::UISystem::LoadVideoUI(game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::VideoUI>())
	{
		return;
	}

	game_module::VideoUI* videoUI = object->GetComponent<game_module::VideoUI>();
	graphics::VideoInfo videoInfo = videoUI->GetVideoInfo();

	if (!std::filesystem::exists(videoInfo.VideoPath))
	{
		spdlog::warn("[UISystem] {} Load failed", videoInfo.VideoPath);
	}
	else
	{
		videoInfo.isReset = true;
		graphics::IVideoObject* videoObject = mGameProcess->mGraphics->CreateVideoObject(videoInfo);
		videoUI->SetVideoObject(videoObject);
	}
}

void fq::game_engine::UISystem::UnloadVideoUI(game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::VideoUI>())
	{
		return;
	}

	game_module::VideoUI* videoUI = object->GetComponent<game_module::VideoUI>();

	graphics::IVideoObject* videoObject = videoUI->GetVideoObject();

	if (videoObject)
	{
		mGameProcess->mGraphics->DeleteVideoObject(videoObject);
		videoUI->SetVideoObject(nullptr);
	}
}

