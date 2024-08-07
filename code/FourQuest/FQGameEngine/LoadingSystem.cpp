#include "LoadingSystem.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

fq::game_engine::LoadingSystem::LoadingSystem()
	:mGameProcess(nullptr)
	, mLoadImage(nullptr)
{}

fq::game_engine::LoadingSystem::~LoadingSystem()
{}

void fq::game_engine::LoadingSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;

	// 1. �ε� ������ �ʿ��� �⺻���� UI�� �ε� 
	loadUI();
}

void fq::game_engine::LoadingSystem::Finalize()
{

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
	mbIsDone = false;

	fq::game_module::ThreadPool::GetInstance()->EnqueueJob([this]() 
		{
			mbIsDone = true;
		});

	// ������ 
	while (!mbIsDone)
	{
		mGameProcess->mGraphics->BeginRender();
		mGameProcess->mGraphics->Render();
		mGameProcess->mGraphics->EndRender();
	}

	mLoadImage->SetIsRender(false);

	mGameProcess->mSceneManager->LoadScene();
}

void fq::game_engine::LoadingSystem::load()
{
	// �ε� ������ 
	mGameProcess->mSceneManager->LoadScene();

}

