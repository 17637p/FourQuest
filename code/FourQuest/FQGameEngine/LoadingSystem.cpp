#include "LoadingSystem.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQClient/LoadingUI.h"
#include "GameProcess.h"
#include "ResourceSystem.h"
#include "UISystem.h"

fq::game_engine::LoadingSystem::LoadingSystem()
	:mGameProcess(nullptr)
	, mLoadImage(nullptr)
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
	//	mGameProcess->mGraphics->DeleteImageObject(mLoadImage);

	for (auto& object : mLoadingUIObject)
	{
		mGameProcess->mUISystem->UnloadImageUI(object.get());
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
	}
	mLoadingUIObject[0]->OnStart();

	// Loading 배경화면 로딩
	fq::graphics::UIInfo info;
	info.Height = 1200;
	info.Width = 2000;
	info.ImagePath = (fq::path::GetInternalPath() / "loading" / "Loading.png").string();
	//mLoadImage = mGameProcess->mGraphics->CreateImageObject(info);
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
	while (check.wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout)
	{
		updateUI();
		mGameProcess->mGraphics->BeginRender();
		mGameProcess->mGraphics->Render();
		mGameProcess->mGraphics->EndRender();
	}

	setRenderUI(false);
	mGameProcess->mEventManager->FireEvent<fq::event::OnLoadScene>({ mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName() });
}

void fq::game_engine::LoadingSystem::updateUI()
{
	float ratio = mGameProcess->mResourceSystem->GetLoadingRatio();
	//spdlog::debug("loading ratio {}", ratio);
	mLoadingUIObject[0]->GetComponent<client::LoadingUI>()->SetProgressBar(ratio);

	for (auto& object : mLoadingUIObject)
	{
		object->OnUpdate(0.f);
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
			
			for (auto imageObject : imageUI->GetImageObjects())
			{
				imageObject->SetIsRender(isRender);
			}
		}

		if (object->HasComponent<game_module::TextUI>())
		{
			auto textUI = object->GetComponent<game_module::TextUI>();
		}
	}
}

