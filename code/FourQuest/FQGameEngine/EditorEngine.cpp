#include "EditorEngine.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGraphics/IFQGraphics.h"

#include "GameProcess.h"
#include "EditorProcess.h"

#include "WindowSystem.h"
#include "ModelSystem.h"
#include "CameraSystem.h"
#include "RenderingSystem.h"

#include "FQGameEngineRegister.h"
#include "GamePlayWindow.h"

fq::game_engine::EditorEngine::EditorEngine()
	:mGameProcess(std::make_unique<GameProcess>())
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
	mGameProcess->mWindowSystem->InitializeEditorType();

	// GameProcess 초기화
	mGameProcess->mInputManager->
		Initialize(mGameProcess->mWindowSystem->GetHWND());

	mGameProcess->mSceneManager->Initialize("example"
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get());

	// 그래픽스 엔진 초기화
	mGameProcess->mGraphics = fq::graphics::EngineExporter().GetEngine();
	HWND hwnd = mGameProcess->mWindowSystem->GetHWND();
	float width = mGameProcess->mWindowSystem->GetScreenWidth();
	float height = mGameProcess->mWindowSystem->GetScreenHeight();
	mGameProcess->mGraphics->Initialize(hwnd, width,height);

	// 시스템 초기화
	mGameProcess->mModelSystem->Initialize(mGameProcess.get());
	mGameProcess->mRenderingSystem->Initialize(mGameProcess.get());
	mGameProcess->mCameraSystem->Initialize(mGameProcess.get());

	// Editor 초기화
	InitializeEditor();

	// Scene 로드 
	mGameProcess->mSceneManager->LoadScene();
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
			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();
				mGameProcess->mGraphics->SetWindowSize(mGameProcess->mWindowSystem->GetScreenWidth()
					, mGameProcess->mWindowSystem->GetScreenHeight());
			}

			auto mode = mEditor->mGamePlayWindow->GetMode();

			float deltaTime = mGameProcess->mTimeManager->Update();

			if (mode == EditorMode::Play)
			{
				// 시간,입력 처리
				mGameProcess->mInputManager->Update();  

				// 물리처리
				mGameProcess->mSceneManager->FixedUpdate(0.f);

				mGameProcess->mSceneManager->Update(deltaTime);
				mGameProcess->mSceneManager->LateUpdate(deltaTime);
			}

			// 시스템 업데이트
			mGameProcess->mRenderingSystem->Update(deltaTime);
			mGameProcess->mCameraSystem->Update();

			// 랜더링 
			mGameProcess->mGraphics->BeginRender();

			mGameProcess->mGraphics->Render();

			mEditor->mImGuiSystem->NewFrame();
			UpdateEditor(deltaTime);

			RenderEditorWinodw();
			mEditor->mImGuiSystem->RenderImGui();
			mGameProcess->mGraphics->EndRender();

			mGameProcess->mSceneManager->PostUpdate();
			if (mGameProcess->mSceneManager->IsEnd())
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
	mGameProcess->mSceneManager->Finalize();
	mGameProcess->mEventManager->RemoveAllHandles();

	mGameProcess->mGraphics->Finalize();

	// Window 종료
	mGameProcess->mWindowSystem->Finalize();
}

void fq::game_engine::EditorEngine::RenderEditorWinodw()
{
	mEditor->mGamePlayWindow->Render();
	mEditor->mHierarchy->Render();
	mEditor->mInspector->Render();
	mEditor->mLogWindow->Render();
	mEditor->mFileDialog->Render();
	mEditor->mMainMenuBar->Render();
}

void fq::game_engine::EditorEngine::InitializeEditor()
{
	// Editor InputManager 초기화
	mEditor->mInputManager->Initialize(mGameProcess->mWindowSystem->GetHWND());

	// System 초기화
	mEditor->mImGuiSystem->Initialize(mGameProcess->mWindowSystem->GetHWND()
		, mGameProcess->mGraphics->GetDivice(), mGameProcess->mGraphics->GetDeviceContext());
	mEditor->mCommandSystem->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mPrefabSystem->Initialize(mGameProcess.get(), mEditor.get());

	// Window 초기화
	mEditor->mInspector->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mHierarchy->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mFileDialog->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mMainMenuBar->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mGamePlayWindow->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mLogWindow->Initialize();
}

void fq::game_engine::EditorEngine::UpdateEditor(float dt)
{
	mEditor->mInputManager->Update();

	mEditor->mGamePlayWindow->UpdateCamera(dt);
	mEditor->mGamePlayWindow->ExcutShortcut();
	mEditor->mMainMenuBar->ExcuteShortcut();
	mEditor->mCommandSystem->ExcuteShortcut();
	mEditor->mHierarchy->ExcuteShortcut();
}

