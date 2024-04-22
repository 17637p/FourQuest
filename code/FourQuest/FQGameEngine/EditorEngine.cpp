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
	// ��Ÿ������ ������ ����մϴ�
	fq::game_module::RegisterMetaData();
	fq::game_engine::RegisterMetaData();

	// ������ â �ʱ�ȭ 
	mGame->mWindowSystem->InitializeEditorType();

	// GameProcess �ʱ�ȭ
	mGame->mInputManager->
		Initialize(mGame->mWindowSystem->GetHWND());

	mGame->mSceneManager->Initialize("example"
		, mGame->mEventManager.get()
		, mGame->mInputManager.get());

	// �׷��Ƚ� ���� �ʱ�ȭ
	mGame->mGraphics = fq::graphics::EngineExporter().GetEngine();
	HWND hwnd = mGame->mWindowSystem->GetHWND();
	float width = mGame->mWindowSystem->GetScreenWidth();
	float height = mGame->mWindowSystem->GetScreenHeight();
	
	mGame->mGraphics->Initialize(hwnd, width,height);

	// Editor �ʱ�ȭ
	InitializeEditor();

	// Scene �ε� 
	mGame->mSceneManager->LoadScene();
}

void fq::game_engine::EditorEngine::Process()
{
	MSG msg;
	bool bIsDone = false;

	while (!bIsDone)
	{
		// ������ �޼����� ó���մϴ�
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
			// ȭ�� ũ�� ����
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
				// �ð�,�Է� ó��
				mGame->mInputManager->Update();  

				// ����ó��
				mGame->mSceneManager->FixedUpdate(0.f);

				mGame->mSceneManager->Update(deltaTime);
				mGame->mSceneManager->LateUpdate(deltaTime);
			}

			mEditor->mImGuiSystem->NewFrame();

			UpdateEditor();

			// ������ 
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

	// Window ����
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
	// Editor InputManager �ʱ�ȭ
	mEditor->mInputManager->Initialize(mGame->mWindowSystem->GetHWND());

	// System �ʱ�ȭ
	mEditor->mImGuiSystem->Initialize(mGame->mWindowSystem->GetHWND()
		, mGame->mGraphics->GetDivice(), mGame->mGraphics->GetDeviceContext());
	mEditor->mCommandSystem->Initialize(mGame.get(), mEditor.get());
	mEditor->mPrefabSystem->Initialize(mGame.get(), mEditor.get());

	// Window �ʱ�ȭ
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

