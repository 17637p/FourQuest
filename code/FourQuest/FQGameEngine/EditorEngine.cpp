#include "EditorEngine.h"

#include "../FQGameModule/GameModule.h"

#include "GameProcess.h"
#include "EditorProcess.h"
#include "TmpD3D.h"

#include "WindowSystem.h"
#include "FQGameEngineRegister.h"

fq::game_engine::EditorEngine::EditorEngine()
	:mGame(std::make_unique<GameProcess>())
	, mEditor(std::make_unique<EditorProcess>())
	, mD3D(std::make_unique<TmpD3D>())
{}

fq::game_engine::EditorEngine::~EditorEngine()
{}

void fq::game_engine::EditorEngine::Initialize()
{
	// 메타데이터 정보를 등록합니다
	fq::game_module::RegisterMetaData();
	fq::game_engine::RegisterMetaData();

	// 윈도우 창 초기화 
	mGame->mWindowSystem->InitializeEditorType();

	// GameProcess 초기화
	mGame->mInputManager->
		Initialize(mGame->mWindowSystem->GetHWND());

	mGame->mSceneManager->Initialize("example"
		, mGame->mEventManager.get()
		, mGame->mInputManager.get());

	// d3d 초기화
	mD3D->Initialize(mGame.get());

	// Editor 초기화
	InitializeEditor();

	// Scene 로드 
	mGame->mSceneManager->LoadScene();
}

void fq::game_engine::EditorEngine::Process()
{
	MSG msg;
	bool bIsDone = false;

	while (!bIsDone)
	{
		// 윈도우 메세지를 처리합니다
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
			if (mGame->mWindowSystem->IsResizedWindow())
			{
				mGame->mWindowSystem->OnResize();
			}

			// 시간,입력 처리
			float deltaTime = mGame->mTimeManager->Update();
			mEditor->mInputManager->Update();
			mGame->mInputManager->Update();

			// 물리처리
			mGame->mSceneManager->FixedUpdate(0.f);
			mGame->mSceneManager->Update(deltaTime);
			mGame->mSceneManager->LateUpdate(deltaTime);

			mEditor->mImGuiSystem->NewFrame();

			// 랜더링 
			mD3D->Clear();
			RenderEditorWinodw();
			mEditor->mImGuiSystem->RenderImGui();
			mD3D->Present();

			mEditor->mCommandSystem->Update();

			mGame->mSceneManager->PostUpdate();
			if (mGame->mSceneManager->IsEnd())
			{
				bIsDone = true;
			}
		}
	}
}

void fq::game_engine::EditorEngine::Finalize()
{
	// Editor Process
	mEditor->mPrefabSystem->Finalize();
	mEditor->mFileDialog->Finalize();
	mEditor->mGamePlayWindow->Finalize();
	mEditor->mInspector->Finalize();
	mEditor->mLogWindow->Finalize();

	// GameProcess
	mGame->mSceneManager->Finalize();
	mGame->mEventManager->RemoveAllHandles();	
	
	// Window 종료
	mGame->mWindowSystem->Finalize();
}

void fq::game_engine::EditorEngine::RenderEditorWinodw()
{
	mEditor->mHierarchy->Render();
	mEditor->mInspector->Render();
	mEditor->mLogWindow->Render();
	mEditor->mFileDialog->Render();
	mEditor->mGamePlayWindow->Render();
	mEditor->mMenuBar->Render();
}

void fq::game_engine::EditorEngine::InitializeEditor()
{
	// Editor InputManager 초기화
	mEditor->mInputManager->Initialize(mGame->mWindowSystem->GetHWND());
	
	// System 초기화
	mEditor->mImGuiSystem->Initialize(mGame->mWindowSystem->GetHWND()
		, mD3D->GetDevice(), mD3D->GetDC());
	mEditor->mCommandSystem->Initialize(mGame.get(), mEditor.get());
	mEditor->mPrefabSystem->Initialize(mGame.get(), mEditor.get());
	
	// Window 초기화
	mEditor->mInspector->Initialize(mGame.get(), mEditor.get());
	mEditor->mHierarchy->Initialize(mGame.get(), mEditor.get());
	mEditor->mFileDialog->Initialize(mGame.get(), mEditor.get(), mD3D->GetDevice());
	mEditor->mMenuBar->Initialize(mGame.get(), mEditor.get());
	mEditor->mLogWindow->Initialize();
}

