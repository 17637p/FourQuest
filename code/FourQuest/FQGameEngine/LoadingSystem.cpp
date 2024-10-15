#include "LoadingSystem.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/SpriteAnimationUI.h"
#include "../FQClient/LoadingUI.h"
#include "GameProcess.h"
#include "ResourceSystem.h"
#include "UISystem.h"
#include "../FQClient/ClientHelper.h"

fq::game_engine::LoadingSystem::LoadingSystem()
	:mGameProcess(nullptr)
	, mLoadingUIObject{}
{}

fq::game_engine::LoadingSystem::~LoadingSystem()
{}

void fq::game_engine::LoadingSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;

	// 1. 로딩 쓰레에 필요한 기본적인 UI를 로드 
	loadUI();
}

void fq::game_engine::LoadingSystem::Finalize()
{
	for (auto& object : mLoadingUIObject)
	{
		mGameProcess->mUISystem->UnloadImageUI(object.get());
		mGameProcess->mUISystem->UnloadSpriteAnimationUI(object.get());
		mGameProcess->mUISystem->UnloadTextUI(object.get());
	}
}

void fq::game_engine::LoadingSystem::loadUI()
{
	// Loading Progress Bar 로딩 
	mLoadingUIObject = mGameProcess->mPrefabManager->LoadPrefab(fq::path::GetResourcePath() / "UI" / "Loading" / "LoadingUI.prefab");
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : mLoadingUIObject)
	{
		object->SetScene(scene);
		mGameProcess->mUISystem->LoadImageUI(object.get());
		mGameProcess->mUISystem->LoadTextUI(object.get());
		mGameProcess->mUISystem->LoadSpriteAnimationUI(object.get());
	}
}

void fq::game_engine::LoadingSystem::ProcessLoading()
{
	mGameProcess->mSceneManager->LoadScene();
	setRenderUI(true);

	auto pool = fq::game_module::ThreadPool::GetInstance();
	auto check = pool->EnqueueJob([this]()
		{
			mGameProcess->mResourceSystem->LoadSceneResource({ mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName() });
		});

	// 랜더링 
	while (check.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float dt = mGameProcess->mTimeManager->Update();
			updateUI(dt);
			mGameProcess->mGraphics->BeginRender();
			mGameProcess->mGraphics->Render();
			mGameProcess->mGraphics->EndRender();
		}
	}

	setRenderUI(false);
	mGameProcess->mEventManager->FireEvent<fq::event::OnLoadScene>({ mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName() });
}

void fq::game_engine::LoadingSystem::updateUI(float dt)
{
	float ratio = mGameProcess->mResourceSystem->GetLoadingRatio();
	auto loadingUI = mLoadingUIObject[0]->GetComponent<client::LoadingUI>();
	loadingUI->SetProgressBar(ratio);

	for (auto& object : mLoadingUIObject)
	{
		object->OnUpdate(dt);
	}
}

void fq::game_engine::LoadingSystem::setRenderUI(bool isRender)
{
	mGameProcess->mGraphics->SetIsRenderObjects(!isRender);

	for (auto& object : mLoadingUIObject)
	{
		if (object->HasComponent<game_module::ImageUI>())
		{
			auto imageUI = object->GetComponent<game_module::ImageUI>();

			auto& imageObjects = imageUI->GetImageObjects();
			auto uiInfomations = imageUI->GetUIInfomations();

			for (int i = 0; i < imageObjects.size(); ++i)
			{
				if (uiInfomations[i].isRender && isRender)
				{
					imageObjects[i]->SetIsRender(isRender);
				}
				else
				{
					imageObjects[i]->SetIsRender(false);
				}
			}
		}

		if (object->HasComponent<game_module::TextUI>())
		{
			auto textUI = object->GetComponent<game_module::TextUI>();

			auto info = textUI->GetTextInfo();
			info.IsRender = isRender;
			textUI->SetTextInfo(info);
		}

		if (object->HasComponent<game_module::SpriteAnimationUI>())
		{
			auto sprite = object->GetComponent<game_module::SpriteAnimationUI>();

			auto info = sprite->GetSpriteInfo();
			info.isRender = isRender;
			sprite->SetSpriteInfo(info);
		}
	}

	if (isRender)
	{
		for (auto& object : mLoadingUIObject)
		{
			object->OnStart();
		}
	}

	auto loadingUI = mLoadingUIObject[0]->GetComponent<client::LoadingUI>();
	auto id = client::helper::RandomGenerator::GetInstance().GetRandomNumber(0, loadingUI->GetGuideSize() - 1);
	loadingUI->SetGuideID(id);
}

