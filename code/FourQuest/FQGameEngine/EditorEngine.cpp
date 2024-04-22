#include "EditorEngine.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGraphics/IFQGraphics.h"

#include "GameProcess.h"
#include "EditorProcess.h"

#include "WindowSystem.h"
#include "FQGameEngineRegister.h"
#include "GamePlayWindow.h"

fq::game_engine::EditorEngine::EditorEngine()
	:mGame(std::make_unique<GameProcess>())
	, mEditor(std::make_unique<EditorProcess>())
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

	// 그래픽스 엔진 초기화
	mGame->mGraphics = fq::graphics::EngineExporter().GetEngine();
	HWND hwnd = mGame->mWindowSystem->GetHWND();
	float width = mGame->mWindowSystem->GetScreenWidth();
	float height = mGame->mWindowSystem->GetScreenHeight();
	
	mGame->mGraphics->Initialize(hwnd, width,height);

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
			if (msg.message == WM_QUIT)
			{
				bIsDone = true;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 화면 크기 조정
			if (mGame->mWindowSystem->IsResizedWindow())
			{
				mGame->mWindowSystem->OnResize();
				mGame->mGraphics->SetWindowSize(mGame->mWindowSystem->GetScreenWidth()
					, mGame->mWindowSystem->GetScreenHeight());
			}

			auto mode = mEditor->mGamePlayWindow->GetMode();

			float deltaTime = mGame->mTimeManager->Update();

			if (mode == EditorMode::Play)
			{
				// 시간,입력 처리
				mGame->mInputManager->Update();  

				// 물리처리
				mGame->mSceneManager->FixedUpdate(0.f);

				mGame->mSceneManager->Update(deltaTime);
				mGame->mSceneManager->LateUpdate(deltaTime);
			}

			mEditor->mImGuiSystem->NewFrame();

			UpdateEditor();

			// 랜더링 
			mGame->mGraphics->BeginRender();
			mGame->mGraphics->Render();

			RenderEditorWinodw();
			mEditor->mImGuiSystem->RenderImGui();

			mGame->mGraphics->EndRender();

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

	mGame->mGraphics->Finalize();

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
	mEditor->mMainMenuBar->Render();
}

void fq::game_engine::EditorEngine::InitializeEditor()
{
	// Editor InputManager 초기화
	mEditor->mInputManager->Initialize(mGame->mWindowSystem->GetHWND());

	// System 초기화
	mEditor->mImGuiSystem->Initialize(mGame->mWindowSystem->GetHWND()
		, mGame->mGraphics->GetDivice(), mGame->mGraphics->GetDeviceContext());
	mEditor->mCommandSystem->Initialize(mGame.get(), mEditor.get());
	mEditor->mPrefabSystem->Initialize(mGame.get(), mEditor.get());

	// Window 초기화
	mEditor->mInspector->Initialize(mGame.get(), mEditor.get());
	mEditor->mHierarchy->Initialize(mGame.get(), mEditor.get());
	mEditor->mFileDialog->Initialize(mGame.get(), mEditor.get());
	mEditor->mMainMenuBar->Initialize(mGame.get(), mEditor.get());
	mEditor->mGamePlayWindow->Initialize(mGame.get(), mEditor.get());
	mEditor->mLogWindow->Initialize();
}

void fq::game_engine::EditorEngine::UpdateEditor()
{
	mEditor->mInputManager->Update();
	mEditor->mGamePlayWindow->ExcutShortcut();
	mEditor->mCommandSystem->ExcuteShortcut();
	mEditor->mHierarchy->ExcuteShortcut();
	mEditor->mMainMenuBar->ExcuteShortcut();
}

