
#include "GameEngine.h"

#include <windows.h>

#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"
#include "WindowSystem.h"

fq::game_engine::GameEngine::GameEngine()
	:mGameProcess(std::make_unique<GameProcess>())
{
}

fq::game_engine::GameEngine::~GameEngine()
{}

void fq::game_engine::GameEngine::Initialize()
{
	// 메타데이터 정보를 등록합니다
	fq::game_module::RegisterMetaData();

	mGameProcess->mWindowSystem->Initialize();

	mGameProcess->mInputManager->Initialize(mGameProcess->mWindowSystem->GetHWND());

	mGameProcess->mSceneManager->Initialize("example"
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get());

	mGameProcess->mSceneManager->LoadScene();
	mGameProcess->mSceneManager->StartScene();
}

void fq::game_engine::GameEngine::Process()
{
	MSG msg;

	bool bIsDone = false;
	while (!bIsDone)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bIsDone = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();
			}

			// 시간, 키입력 처리 
			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();

			// 물리처리
			mGameProcess->mSceneManager->FixedUpdate(0.f);

			mGameProcess->mSceneManager->Update(deltaTime);
			mGameProcess->mSceneManager->LateUpdate(deltaTime);

			mGameProcess->mSceneManager->PostUpdate();
			if (mGameProcess->mSceneManager->IsEnd())
			{
				bIsDone = true;
			}
		}
	}
}

void fq::game_engine::GameEngine::Finalize()
{
	mGameProcess->mWindowSystem->Finalize();
}

