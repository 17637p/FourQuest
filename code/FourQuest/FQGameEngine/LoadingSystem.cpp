#include "LoadingSystem.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"
#include "ResourceSystem.h"

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
	mGameProcess->mGraphics->DeleteImageObject(mLoadImage);
}

void fq::game_engine::LoadingSystem::loadUI()
{
	fq::graphics::UIInfo info;
	info.Height = 1080;
	info.Width = 1920;
	info.ImagePath = (fq::path::GetInternalPath() / "loading" / "Loading.png").string();
	mLoadImage = mGameProcess->mGraphics->CreateImageObject(info);
}

void fq::game_engine::LoadingSystem::ProcessLoading()
{
	mGameProcess->mSceneManager->LoadScene();
	mGameProcess->mGraphics->SetIsRenderObjects(false);

	auto pool = fq::game_module::ThreadPool::GetInstance();

	pool->EnqueueJob([this]()
		{
			mGameProcess->mResourceSystem->LoadSceneResource({ mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName() });
		});

	// 랜더링 
	while (!pool->IsAllWaitState())
	{
		mGameProcess->mGraphics->BeginRender();
		mGameProcess->mGraphics->Render();
		mGameProcess->mGraphics->EndRender();
	}

	mLoadImage->SetIsRender(false);
	mGameProcess->mGraphics->SetIsRenderObjects(true);

	mGameProcess->mEventManager->FireEvent<fq::event::OnLoadScene>({ mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName() });
}

