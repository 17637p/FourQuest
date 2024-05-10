#include "EditorEngine.h"

#include <algorithm>

#include "../FQGameModule/GameModule.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQphysics/IFQPhysics.h"

#include "GameProcess.h"
#include "EditorProcess.h"

#include "WindowSystem.h"
#include "ModelSystem.h"
#include "CameraSystem.h"
#include "RenderingSystem.h"
#include "LightSystem.h"
#include "PhysicsSystem.h"
#include "SoundSystem.h"

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
	// ��Ÿ������ ������ ����մϴ�
	fq::game_module::RegisterMetaData();
	fq::game_engine::RegisterMetaData();

	// ������ â �ʱ�ȭ 
	mGameProcess->mWindowSystem->InitializeEditorType();

	// GameProcess �ʱ�ȭ
	mGameProcess->mInputManager->
		Initialize(mGameProcess->mWindowSystem->GetHWND());
	
	mGameProcess->mSceneManager->Initialize("example"
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get()
		, mGameProcess->mPrefabManager.get());
	
	mGameProcess->mSoundManager->Initialize();

	// �׷��Ƚ� ���� �ʱ�ȭ
	mGameProcess->mGraphics = fq::graphics::EngineExporter().GetEngine();
	HWND hwnd = mGameProcess->mWindowSystem->GetHWND();
	UINT width = mGameProcess->mWindowSystem->GetScreenWidth();
	UINT height = mGameProcess->mWindowSystem->GetScreenHeight();
	mGameProcess->mGraphics->Initialize(hwnd, width, height);

	// ���� ���� �ʱ�ȭ
	mGameProcess->mPhysics = fq::physics::EngineExporter().GetEngine();
	fq::physics::PhysicsEngineInfo info;
	info.gravity = { 0.f,-10.f,0.f };
	mGameProcess->mPhysics->Initialize(info);

	// �ý��� �ʱ�ȭ
	mGameProcess->mRenderingSystem->Initialize(mGameProcess.get());
	mGameProcess->mCameraSystem->Initialize(mGameProcess.get());
	mGameProcess->mPhysicsSystem->Initialize(mGameProcess.get());
	mGameProcess->mLightSystem->Initialize(mGameProcess.get());
	mGameProcess->mSoundSystem->Initialize(mGameProcess.get());

	// Editor �ʱ�ȭ
 	InitializeEditor();

	// Scene �ε� 
	mGameProcess->mSceneManager->LoadScene();
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
			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();

				unsigned short width = std::max(mGameProcess->mWindowSystem->GetScreenWidth(), 1u);
				unsigned short hegiht =std::max(mGameProcess->mWindowSystem->GetScreenHeight(),1u);
				mGameProcess->mGraphics->SetWindowSize(width, hegiht);
			}

			auto mode = mEditor->mGamePlayWindow->GetMode();

			// �ð�,�Է� ó��
			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();
			mGameProcess->mSoundManager->Update();

			if (mode == EditorMode::Play)
			{
				// ����ó��
				mGameProcess->mSceneManager->FixedUpdate(deltaTime);
				mGameProcess->mPhysicsSystem->SinkToPhysicsScene();
				mGameProcess->mPhysics->Update(deltaTime);
				mGameProcess->mPhysics->FinalUpdate();
				mGameProcess->mPhysicsSystem->SinkToGameScene();

				mGameProcess->mSceneManager->Update(deltaTime);
				mGameProcess->mSceneManager->LateUpdate(deltaTime);
			}

			// �ý��� ������Ʈ
			mGameProcess->mRenderingSystem->Update(deltaTime);
			mGameProcess->mLightSystem->Update();
			mGameProcess->mCameraSystem->Update();

			// ������ 
			mGameProcess->mGraphics->BeginRender();
			mEditor->mDebugSystem->Render();
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
	mGameProcess->mSceneManager->UnloadScene();

	// Editor Process
	mEditor->mPrefabSystem->Finalize();
	mEditor->mFileDialog->Finalize();
	mEditor->mGamePlayWindow->Finalize();
	mEditor->mInspector->Finalize();
	mEditor->mLogWindow->Finalize();
	mEditor->mImGuiSystem->Finalize();

	// SystemFinalize
	mGameProcess->mGraphics->Finalize();
	fq::graphics::EngineExporter().DeleteEngine(mGameProcess->mGraphics);
	fq::physics::EngineExporter().DeleteEngine(mGameProcess->mPhysics);

	// GameProcess
	mGameProcess->mSceneManager->Finalize();
	mGameProcess->mEventManager->RemoveAllHandles();

	fq::game_module::ObjectPool::Finalize();

	// Window ����
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
	mEditor->mCollisionMatrixWindow->Render();
}

void fq::game_engine::EditorEngine::InitializeEditor()
{
	// Editor InputManager �ʱ�ȭ
	mEditor->mInputManager->Initialize(mGameProcess->mWindowSystem->GetHWND());

	// System �ʱ�ȭ
	mEditor->mImGuiSystem->Initialize(mGameProcess->mWindowSystem->GetHWND()
		, mGameProcess->mGraphics->GetDivice(), mGameProcess->mGraphics->GetDeviceContext());
	mEditor->mCommandSystem->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mPrefabSystem->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mModelSystem->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mDebugSystem->Initialize(mGameProcess.get());

	// Window �ʱ�ȭ
	mEditor->mInspector->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mHierarchy->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mFileDialog->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mMainMenuBar->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mGamePlayWindow->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mLogWindow->Initialize(mGameProcess.get());
	mEditor->mCollisionMatrixWindow->Initialize(mGameProcess.get());
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

