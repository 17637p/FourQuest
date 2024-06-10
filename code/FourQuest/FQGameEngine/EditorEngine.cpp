#include "EditorEngine.h"

#include <algorithm>

#include "../FQClient/Client.h"
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
#include "AnimationSystem.h"
#include "PhysicsSystem.h"
#include "SoundSystem.h"
#include "ParticleSystem.h"

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
	fq::client::RegisterMetaData();

	// ������ â �ʱ�ȭ 
	mGameProcess->mWindowSystem->InitializeEditorType();

	// GameProcess �ʱ�ȭ
	mGameProcess->mInputManager->
		Initialize(mGameProcess->mWindowSystem->GetHWND());

	mGameProcess->mSceneManager->Initialize("PlayerTest2"
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
	mGameProcess->mAnimationSystem->Initialize(mGameProcess.get());
	mGameProcess->mParticleSystem->Initialize(mGameProcess.get());

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
				unsigned short hegiht = std::max(mGameProcess->mWindowSystem->GetScreenHeight(), 1u);
				mGameProcess->mGraphics->SetWindowSize(width, hegiht);
			}

			auto mode = mEditor->mGamePlayWindow->GetMode();

			// �ð�,�Է� ó��
			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();
			mGameProcess->mSoundManager->Update();

			if (mode == EditorMode::Play)
			{
				static float accmulator = 0.f;
				constexpr float fixedDeltaTime = 1.f / 60.f;

				accmulator += deltaTime;

				while (accmulator >= fixedDeltaTime)
				{
					// ����ó��
					mGameProcess->mSceneManager->FixedUpdate(fixedDeltaTime);
					mGameProcess->mPhysicsSystem->SinkToPhysicsScene();
					mGameProcess->mPhysics->Update(fixedDeltaTime);
					mGameProcess->mPhysics->FinalUpdate();
					mGameProcess->mPhysicsSystem->SinkToGameScene();

					accmulator -= fixedDeltaTime;
				}

				// Scene Update
				mGameProcess->mSceneManager->Update(deltaTime);

				// Animation Update
				mGameProcess->mAnimationSystem->UpdateAnimation(deltaTime);

				// Scene Late Update
				mGameProcess->mSceneManager->LateUpdate(deltaTime);
			}

			// �ý��� ������Ʈ
			mGameProcess->mParticleSystem->Update(deltaTime);
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
	mEditor->mAnimatorWindow->Finalize();

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
	mEditor->mPhysicsAnimatorWindow->Render();
	mEditor->mSkyBoxWindow->Render();
	mEditor->mAnimatorWindow->Render();
	mEditor->mSettingWindow->Render();
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
	mEditor->mPhysicsAnimatorWindow->Initialize(mGameProcess.get());
	mEditor->mSkyBoxWindow->Initialize(mGameProcess.get());
	mEditor->mAnimatorWindow->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mSettingWindow->Initialize(mGameProcess.get(), mEditor.get());
}

void fq::game_engine::EditorEngine::UpdateEditor(float dt)
{
	mEditor->mInputManager->Update();

	mEditor->mGamePlayWindow->UpdateCamera(dt);
	mEditor->mGamePlayWindow->ExcutShortcut();
	mEditor->mMainMenuBar->ExcuteShortcut();
	mEditor->mCommandSystem->ExcuteShortcut();
	mEditor->mHierarchy->ExcuteShortcut();
	mEditor->mPhysicsAnimatorWindow->UpdateAnimation(dt);
}

