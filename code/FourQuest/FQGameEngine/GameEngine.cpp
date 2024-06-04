
#include "GameEngine.h"

#include <windows.h>

#include "../FQClient/Client.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQphysics/IFQPhysics.h"
#include "FQGameEngineRegister.h"

#include "GameProcess.h"
#include "WindowSystem.h"
#include "SoundSystem.h"
#include "RenderingSystem.h"
#include "PhysicsSystem.h"
#include "LightSystem.h"
#include "CameraSystem.h"
#include "AnimationSystem.h"

fq::game_engine::GameEngine::GameEngine()
	:mGameProcess(std::make_unique<GameProcess>())
{
}

fq::game_engine::GameEngine::~GameEngine()
{}

void fq::game_engine::GameEngine::Initialize()
{
	// ��Ÿ������ ������ ����մϴ�
	fq::game_module::RegisterMetaData();
	fq::game_engine::RegisterMetaData();
	fq::client::RegisterMetaData();

	// ������ â�� �ʱ�ȭ
	mGameProcess->mWindowSystem->Initialize();

	// GameProcess �ʱ�ȭ
	mGameProcess->mInputManager->Initialize(mGameProcess->mWindowSystem->GetHWND());

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
	mGameProcess->mLightSystem->Initialize(mGameProcess.get());
	mGameProcess->mPhysicsSystem->Initialize(mGameProcess.get());
	mGameProcess->mSoundSystem->Initialize(mGameProcess.get());
	mGameProcess->mAnimationSystem->Initialize(mGameProcess.get());

	// ���� �ε��մϴ� 
	mGameProcess->mSceneManager->LoadScene();

	mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Game);

	// ������ �����ϹǷ� StartScene ȣ��
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
			// ȭ�� ũ�� ����
			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();

				unsigned short width = std::max(mGameProcess->mWindowSystem->GetScreenWidth(), 1u);
				unsigned short hegiht = std::max(mGameProcess->mWindowSystem->GetScreenHeight(), 1u);
				mGameProcess->mGraphics->SetWindowSize(width, hegiht);
			}

			// �ð�, Ű�Է� ó�� 
			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();
			
			static float accmulator = 0.f;
			static float fixedDeltaTime = 1.f / 60.f;

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

			mGameProcess->mPhysicsSystem->Update(deltaTime);

			// Scene Update
			mGameProcess->mSceneManager->Update(deltaTime);

			// Animation Update
			mGameProcess->mAnimationSystem->UpdateAnimation(deltaTime);

			// Scene Late Update
			mGameProcess->mSceneManager->LateUpdate(deltaTime);

			// �ý��� ������Ʈ
			mGameProcess->mRenderingSystem->Update(deltaTime);
			mGameProcess->mLightSystem->Update();
			mGameProcess->mCameraSystem->Update();

			// ������
			mGameProcess->mGraphics->BeginRender();
			mGameProcess->mGraphics->Render();
			mGameProcess->mGraphics->EndRender();

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
	mGameProcess->mSceneManager->UnloadScene();

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

