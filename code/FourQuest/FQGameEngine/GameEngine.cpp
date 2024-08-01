
#include "GameEngine.h"

#include <windows.h>

#include "../FQClient/Client.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModuleRegister.h"
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
#include "TrailSystem.h"
#include "PathFindingSystem.h"
#include "UISystem.h"
#include "LightProbeSystem.h"

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
	// 메타데이터 정보를 등록합니다
	fq::game_module::RegisterMetaData();
	fq::game_engine::RegisterMetaData();
	fq::client::RegisterMetaData();

	// 쓰레드 풀 생성
	fq::game_module::ThreadPool::Initialize();

	// 윈도우 창을 초기화
	mGameProcess->mWindowSystem->Initialize();

	// GameProcess 초기화
	mGameProcess->mInputManager->Initialize(mGameProcess->mWindowSystem->GetHWND());

	constexpr const char* StartSceneName = "Scene4";

	mGameProcess->mSceneManager->Initialize(StartSceneName
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get()
		, mGameProcess->mPrefabManager.get()
		, mGameProcess->mScreenManager.get()
		, mGameProcess->mTimeManager.get());

	mGameProcess->mSoundManager->Initialize();
	mGameProcess->mScreenManager->Initialize(mGameProcess->mEventManager.get());

	// 그래픽스 엔진 초기화
	mGameProcess->mGraphics = fq::graphics::EngineExporter().GetEngine();
	HWND hwnd = mGameProcess->mWindowSystem->GetHWND();
	UINT width = mGameProcess->mWindowSystem->GetScreenWidth();
	UINT height = mGameProcess->mWindowSystem->GetScreenHeight();
	mGameProcess->mGraphics->Initialize(hwnd, width, height);

	// 물리 엔진 초기화
	mGameProcess->mPhysics = fq::physics::EngineExporter().GetEngine();
	fq::physics::PhysicsEngineInfo info;
	info.gravity = { 0.f,-10.f,0.f };
	mGameProcess->mPhysics->Initialize(info);

	// 시스템 초기화
	mGameProcess->mRenderingSystem->Initialize(mGameProcess.get());
	mGameProcess->mCameraSystem->Initialize(mGameProcess.get());
	mGameProcess->mPhysicsSystem->Initialize(mGameProcess.get());
	mGameProcess->mLightSystem->Initialize(mGameProcess.get());
	mGameProcess->mSoundSystem->Initialize(mGameProcess.get());
	mGameProcess->mAnimationSystem->Initialize(mGameProcess.get());
	mGameProcess->mParticleSystem->Initialize(mGameProcess.get());
	mGameProcess->mDecalSystem->Initialize(mGameProcess.get());
	mGameProcess->mTrailSystem->Initialize(mGameProcess.get());
	mGameProcess->mUISystem->Initialize(mGameProcess.get());
	mGameProcess->mPathFindgingSystem->Initialize(mGameProcess.get());

	// 씬을 로드합니다 
	mGameProcess->mSceneManager->LoadScene();

	mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Game);

	// 게임을 시작하므로 StartScene 호출
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
			//							시간 입력 처리								//
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
				mGameProcess->mSceneManager->FixedUpdate(fixedDeltaTime);
				mGameProcess->mPhysicsSystem->SinkToPhysicsScene();
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

			// PathFindingSystem Update
			mGameProcess->mPathFindgingSystem->Update(deltaTime);

			// Scene Late Update
			mGameProcess->mSceneManager->LateUpdate(deltaTime);

			//////////////////////////////////////////////////////////////////////////
			//							System Process								//
			//////////////////////////////////////////////////////////////////////////

			// mGameProcess->mParticleSystem->Update(deltaTime);
			mGameProcess->mDecalSystem->Update(deltaTime);
			mGameProcess->mTrailSystem->Update(deltaTime);
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
	//System
	mGameProcess->mLightProbeSystem->Finalize();

	mGameProcess->mSceneManager->UnloadScene();

	mGameProcess->mGraphics->Finalize();
	fq::graphics::EngineExporter().DeleteEngine(mGameProcess->mGraphics);
	fq::physics::EngineExporter().DeleteEngine(mGameProcess->mPhysics);

	// GameProcess
	mGameProcess->mSceneManager->Finalize();
	mGameProcess->mEventManager->RemoveAllHandles();

	fq::game_module::ObjectPool::Finalize();
	fq::game_module::ThreadPool::Finalize();

	// Window 종료
	mGameProcess->mWindowSystem->Finalize();
}

