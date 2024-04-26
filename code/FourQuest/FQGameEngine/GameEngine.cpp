
#include "GameEngine.h"

#include <windows.h>

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQphysics/IFQPhysics.h"
#include "FQGameEngineRegister.h"

#include "GameProcess.h"
#include "WindowSystem.h"
#include "RenderingSystem.h"
#include "CameraSystem.h"

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

	// 윈도우 창을 초기화
	mGameProcess->mWindowSystem->Initialize();

	// GameProcess 초기화
	mGameProcess->mInputManager->Initialize(mGameProcess->mWindowSystem->GetHWND());

	mGameProcess->mSceneManager->Initialize("example"
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get());

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

	// 씬을 로드합니다 
	mGameProcess->mSceneManager->LoadScene();

	// 게임을 시작하므로 StartScene 호출
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
			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();
				mGameProcess->mGraphics->SetWindowSize(mGameProcess->mWindowSystem->GetScreenWidth()
					, mGameProcess->mWindowSystem->GetScreenHeight());
			}

			// 시간, 키입력 처리 
			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();

			// 물리처리
			mGameProcess->mSceneManager->FixedUpdate(0.f);
			mGameProcess->mPhysics->Update(deltaTime);

			mGameProcess->mSceneManager->Update(deltaTime);
			mGameProcess->mSceneManager->LateUpdate(deltaTime);

			// 시스템 업데이트
			mGameProcess->mRenderingSystem->Update(deltaTime);
			mGameProcess->mCameraSystem->Update();

			// 랜더링
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
	mGameProcess->mGraphics->Finalize();
	
	// GameProcess
	mGameProcess->mSceneManager->Finalize();
	mGameProcess->mEventManager->RemoveAllHandles();

	fq::graphics::EngineExporter().DeleteEngine(mGameProcess->mGraphics);
	fq::physics::EngineExporter().DeleteEngine(mGameProcess->mPhysics);

	mGameProcess->mWindowSystem->Finalize();
}

