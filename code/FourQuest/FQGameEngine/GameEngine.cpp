
#include "GameEngine.h"

#include <windows.h>

#include "../FQClient/Client.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQphysics/IFQPhysics.h"
#include "FQGameEngineRegister.h"

#include "GameProcess.h"
#include "WindowSystem.h"
#include "ModelSystem.h"
#include "CameraSystem.h"
#include "RenderingSystem.h"
#include "LightSystem.h"
#include "AnimationSystem.h"
#include "PhysicsSystem.h"
#include "SoundSystem.h"
#include "ParticleSystem.h"
#include "DecalSystem.h"
#include "PathFindingSystem.h"
#include "UISystem.h"

#include "FQGameEngineRegister.h"
#include "GamePlayWindow.h"

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

	constexpr const char* StartSceneName = "Level2";

	mGameProcess->mSceneManager->Initialize(StartSceneName
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get()
		, mGameProcess->mPrefabManager.get()
		, mGameProcess->mScreenManager.get());

	mGameProcess->mSoundManager->Initialize();
	mGameProcess->mScreenManager->Initialize(mGameProcess->mEventManager.get());

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
	mGameProcess->mDecalSystem->Initialize(mGameProcess.get());
	mGameProcess->mUISystem->Initialize(mGameProcess.get());
	mGameProcess->mPathFindgingSystem->Initialize(mGameProcess.get());

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
		//////////////////////////////////////////////////////////////////////////
		//							Window Message								//
		//////////////////////////////////////////////////////////////////////////

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
			//////////////////////////////////////////////////////////////////////////
			//							Resize Window								//
			//////////////////////////////////////////////////////////////////////////

			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();

				unsigned short width = std::max(mGameProcess->mWindowSystem->GetScreenWidth(), 1u);
				unsigned short hegiht = std::max(mGameProcess->mWindowSystem->GetScreenHeight(), 1u);
				mGameProcess->mGraphics->SetWindowSize(width, hegiht);
				mGameProcess->mEventManager->FireEvent<fq::event::SetScreenSize>({ width,hegiht });
			}

			//////////////////////////////////////////////////////////////////////////
			//							�ð� �Է� ó��								//
			//////////////////////////////////////////////////////////////////////////

			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();
			mGameProcess->mSoundManager->Update();

			//////////////////////////////////////////////////////////////////////////
			//							Physics Process								//
			//////////////////////////////////////////////////////////////////////////
			
			static float accmulator = 0.f;
			constexpr float fixedDeltaTime = 1.f / 60.f;
			accmulator += deltaTime;

			bool onFixedUpdtae = false;
			while (accmulator >= fixedDeltaTime)
			{
				accmulator -= fixedDeltaTime;
				onFixedUpdtae = true;

				mGameProcess->mPhysicsSystem->SinkToPhysicsScene();
				mGameProcess->mSceneManager->FixedUpdate(fixedDeltaTime);
				mGameProcess->mPhysics->Update(fixedDeltaTime);
				mGameProcess->mPhysics->FinalUpdate();
				mGameProcess->mPhysicsSystem->SinkToGameScene();
				mGameProcess->mPhysicsSystem->ProcessCallBack();
			}

			if (onFixedUpdtae)
			{
				mGameProcess->mSceneManager->GetCurrentScene()->CleanUp();
			}

			//////////////////////////////////////////////////////////////////////////
			//							Scene Process								//
			//////////////////////////////////////////////////////////////////////////

			// Scene Update
			mGameProcess->mSceneManager->Update(deltaTime);

			// Animation Update
			mGameProcess->mAnimationSystem->UpdateAnimation(deltaTime);

			// Scene Late Update
			mGameProcess->mSceneManager->LateUpdate(deltaTime);
		
			//////////////////////////////////////////////////////////////////////////
			//							System Process								//
			//////////////////////////////////////////////////////////////////////////
			
			mGameProcess->mDecalSystem->Update(deltaTime);
			mGameProcess->mRenderingSystem->Update(deltaTime);
			mGameProcess->mLightSystem->Update();
			mGameProcess->mCameraSystem->Update();
			mGameProcess->mPathFindgingSystem->Update(deltaTime);

			//////////////////////////////////////////////////////////////////////////
			//							Rendering Process							//
			//////////////////////////////////////////////////////////////////////////

			mGameProcess->mGraphics->BeginRender();
			mGameProcess->mGraphics->Render();
			mGameProcess->mGraphics->EndRender();

			//////////////////////////////////////////////////////////////////////////
			//							Post Scene Process							//
			//////////////////////////////////////////////////////////////////////////

			mGameProcess->mPhysicsSystem->PostUpdate();
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

