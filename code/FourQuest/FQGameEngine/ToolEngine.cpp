#include "ToolEngine.h"

#include "../FQGameModule/GameModule.h"

#include "GameProcess.h"
#include "Editor.h"

#include "WindowSystem.h"


fq::game_engine::ToolEngine::ToolEngine()
	:mGameProcess(std::make_unique<GameProcess>())
	,mEditor(std::make_unique<Editor>())
{}

fq::game_engine::ToolEngine::~ToolEngine()
{}

void fq::game_engine::ToolEngine::Initialize()
{
	// 윈도우 창 초기화 
	mGameProcess->mWindowSystem->Initialize();

	// GameProcess 초기화
	mGameProcess->mInputManager->
		Initialize(mGameProcess->mWindowSystem->GetHWND());

	mGameProcess->mSceneManager->Initialize("example", mGameProcess->mEventManager.get());

	// Editor 초기화
	mEditor->Initialize(mGameProcess.get());
}

void fq::game_engine::ToolEngine::Process()
{
	MSG msg;

	bool bIsDone = false;
	while (!bIsDone)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				bIsDone = true;
			}
		}
		else
		{
			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();
			}

			mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();
			mEditor->Update();
			mEditor->NewFrame();
			mEditor->Render();

			mGameProcess->mSceneManager->GetCurrentScene()->CleanUp();
		}
	}
}

void fq::game_engine::ToolEngine::Finalize()
{
	mGameProcess->mSceneManager->Finalize();
}

