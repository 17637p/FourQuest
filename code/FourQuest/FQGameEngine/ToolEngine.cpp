#include "ToolEngine.h"

#include "../FQGameModule/GameModule.h"

#include "GameProcess.h"
#include "Editor.h"

#include "WindowSystem.h"
#include "FQGameEngineRegister.h"

fq::game_engine::ToolEngine::ToolEngine()
	:mGameProcess(std::make_unique<GameProcess>())
	, mEditor(std::make_unique<Editor>())
{}

fq::game_engine::ToolEngine::~ToolEngine()
{}

void fq::game_engine::ToolEngine::Initialize()
{
	// 메타데이터 정보를 등록합니다
	fq::game_module::RegisterMetaData();
	fq::game_engine::RegisterMetaData();

	// 윈도우 창 초기화 
	mGameProcess->mWindowSystem->InitializeEditorType();

	// GameProcess 초기화
	mGameProcess->mInputManager->
		Initialize(mGameProcess->mWindowSystem->GetHWND());

	mGameProcess->mSceneManager->Initialize("example"
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get());

	// Editor 초기화
	mEditor->Initialize(mGameProcess.get());

	// Scene 로드 
	mGameProcess->mSceneManager->LoadScene();

	// Scene 시작
	mGameProcess->mSceneManager->StartScene();
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
			// 화면 크기 조정
			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();
			}
			
			// 시간,입력 처리
			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();

			// 물리처리
			mGameProcess->mSceneManager->FixedUpdate(0.f);
			
			mGameProcess->mSceneManager->Update(deltaTime);
			mGameProcess->mSceneManager->LateUpdate(deltaTime);

			mEditor->Update();
			mEditor->NewFrame();
			mEditor->Render();

			mGameProcess->mSceneManager->PostUpdate();
			if (mGameProcess->mSceneManager->IsEnd())
			{
				bIsDone = true;
			}
		}
	}
}

void fq::game_engine::ToolEngine::Finalize()
{
	mEditor->Finalize();
	mGameProcess->mSceneManager->Finalize();



	mGameProcess->mEventManager->RemoveAllHandles();
}

