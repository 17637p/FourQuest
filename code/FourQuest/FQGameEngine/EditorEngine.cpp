#include "EditorEngine.h"

#include <algorithm>
#include <spdlog/stopwatch.h>

#include "../FQClient/Client.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/GameModuleRegister.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQphysics/IFQPhysics.h"
#include "../FQClient/GameVaribleHelper.h"

#include "GameProcess.h"
#include "EditorProcess.h"
#include "EditorHelper.h"

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
#include "UISystem.h"
#include "PathFindingSystem.h"
#include "LoadingSystem.h"
#include "ResourceSystem.h"
#include "StateEventSystem.h"

#include "FQGameEngineRegister.h"
#include "GamePlayWindow.h"

fq::game_engine::EditorEngine::EditorEngine()
	:mGameProcess(std::make_unique<GameProcess>())
	, mEditor(std::make_unique<EditorProcess>())
{}

fq::game_engine::EditorEngine::~EditorEngine()
{}

bool fq::game_engine::EditorEngine::Initialize()
{
	// COM
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	// 메타데이터 정보를 등록합니다
	fq::game_module::RegisterMetaData();
	fq::game_engine::RegisterMetaData();
	fq::client::RegisterMetaData();

	// 게임 전역 변수 로드 
	client::GameVaribleHelper::Load();

	// 쓰레드 풀 생성
	fq::game_module::ThreadPool::Initialize();

	// 윈도우 창 초기화 
	UINT ScreenWidth = 1920;
	UINT ScreenHeight = 1080;
	mGameProcess->mWindowSystem->InitializeEditorType(ScreenWidth, ScreenHeight);

	// GameProcess 초기화
	mGameProcess->mInputManager->
		Initialize(mGameProcess->mWindowSystem->GetHWND());

	std::string startSceneName = EditorHelper::GetStartSceneName();

	mGameProcess->mSceneManager->Initialize(startSceneName
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get()
		, mGameProcess->mPrefabManager.get()
		, mGameProcess->mScreenManager.get()
		, mGameProcess->mTimeManager.get()
		, mGameProcess->mSoundManager.get());

	mGameProcess->mSoundManager->Initialize();

	// 그래픽스 엔진 초기화
	mGameProcess->mGraphics = fq::graphics::EngineExporter().GetEngine();
	HWND hwnd = mGameProcess->mWindowSystem->GetHWND();
	UINT width = mGameProcess->mWindowSystem->GetScreenWidth();
	UINT height = mGameProcess->mWindowSystem->GetScreenHeight();
	mGameProcess->mGraphics->Initialize(hwnd, width, height, fq::graphics::EPipelineType::Deferred);

	mGameProcess->mScreenManager->Initialize(mGameProcess->mEventManager.get(), width, height, hwnd);

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
	mGameProcess->mLoadingSystem->Initialize(mGameProcess.get());
	mGameProcess->mResourceSystem->Initialize(mGameProcess.get());
	mGameProcess->mStateEventSystem->Initialize(mGameProcess.get());

	// Editor 초기화
	InitializeEditor();

	// Scene 로드 
	return mGameProcess->mLoadingSystem->ProcessLoading();
}

void fq::game_engine::EditorEngine::Process()
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
				unsigned short height = std::max(mGameProcess->mWindowSystem->GetScreenHeight(), 1u);

				mGameProcess->mGraphics->SetWindowSize(width, height);
				mGameProcess->mEventManager->FireEvent<fq::event::SetScreenSize>({ width,height });
			}
			auto mode = mEditor->mGamePlayWindow->GetMode();

			//////////////////////////////////////////////////////////////////////////
			//							시간 입력 처리								//
			//////////////////////////////////////////////////////////////////////////


			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update(deltaTime);
			mGameProcess->mSoundManager->Update();


			if (mode == EditorMode::Play)
			{
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
					mGameProcess->mPathFindgingSystem->Update(fixedDeltaTime);
					mGameProcess->mSceneManager->FixedUpdate(fixedDeltaTime);
					mGameProcess->mPhysicsSystem->SinkToPhysicsScene();
					mGameProcess->mPhysics->Update(fixedDeltaTime);
					mGameProcess->mPhysics->FinalUpdate();
					mGameProcess->mPhysicsSystem->SinkToGameScene();
					mGameProcess->mPhysicsSystem->ProcessCallBack();
				
				}

				if (onFixedUpdtae)
				{
					mGameProcess->mSceneManager->GetCurrentScene()->CleanUp(true);
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
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				//							Edit, Pause Mode							//
				//////////////////////////////////////////////////////////////////////////
				mGameProcess->mSceneManager->GetCurrentScene()->CleanUp(false);
				EditorHelper::UpdateEditorMode(mGameProcess.get(), deltaTime);
			}

			//////////////////////////////////////////////////////////////////////////
			//							Bind Process								//
			//////////////////////////////////////////////////////////////////////////

			mGameProcess->mUISystem->Update(deltaTime);
			mGameProcess->mDecalSystem->Update(deltaTime);
			mGameProcess->mTrailSystem->Update(deltaTime);
			mGameProcess->mRenderingSystem->Update(deltaTime);
			mGameProcess->mLightSystem->Update();
			mGameProcess->mCameraSystem->Update();
			mGameProcess->mStateEventSystem->Update(deltaTime);

			//////////////////////////////////////////////////////////////////////////
			//							Rendering Process							//
			//////////////////////////////////////////////////////////////////////////

			// 랜더링 
			mGameProcess->mGraphics->BeginRender();
			mEditor->mDebugSystem->Render();
			mGameProcess->mGraphics->Render();

			mEditor->mImGuiSystem->NewFrame();
			UpdateEditor(mGameProcess->mTimeManager->GetDeltaTime());
			RenderEditorWinodw();
			mEditor->mImGuiSystem->RenderImGui();

			mGameProcess->mGraphics->EndRender();

			//////////////////////////////////////////////////////////////////////////
			//							Post Scene Proeces							//
			//////////////////////////////////////////////////////////////////////////

			mGameProcess->mPhysicsSystem->PostUpdate();
			mGameProcess->mSceneManager->PostUpdate();

			//////////////////////////////////////////////////////////////////////////
			//							Scene 변경 처리								//
			//////////////////////////////////////////////////////////////////////////
			if (mGameProcess->mSceneManager->IsChangeScene())
			{
				mGameProcess->mSceneManager->UnloadScene();
				bool IsExitGame = mGameProcess->mLoadingSystem->ProcessLoading();
				if (IsExitGame) return;
				mGameProcess->mSceneManager->StartScene();
			}

			//////////////////////////////////////////////////////////////////////////
			//							게임 종료 처리								//
			//////////////////////////////////////////////////////////////////////////
			if (mGameProcess->mSceneManager->IsEnd())
			{
				bIsDone = true;
			}
		}
	}
}

void fq::game_engine::EditorEngine::Finalize()
{
	fq::game_module::ThreadPool::Finalize();

	EditorHelper::SetStartSceneName(mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName());
	EditorHelper::SaveEditorSetting(mEditor.get());

	mGameProcess->mSceneManager->UnloadScene();
	mGameProcess->mLoadingSystem->Finalize();
	mGameProcess->mSoundSystem->Finalize();
	mGameProcess->mUISystem->Finalize();

	// Editor Process
	mEditor->mImageSystem->Finalize();
	mEditor->mFileDialog->Finalize();
	mEditor->mGamePlayWindow->Finalize();
	mEditor->mInspector->Finalize();
	mEditor->mLogWindow->Finalize();
	mEditor->mImGuiSystem->Finalize();
	mEditor->mAnimatorWindow->Finalize();
	mEditor->mArticulationHierarchy->Finalize();
	mEditor->mClothEditorWindow->Finalize();

	// SystemFinalize
	mGameProcess->mGraphics->Finalize();
	fq::graphics::EngineExporter().DeleteEngine(mGameProcess->mGraphics);
	fq::physics::EngineExporter().DeleteEngine(mGameProcess->mPhysics);

	// GameProcess 
	mGameProcess->mSceneManager->Finalize();
	mGameProcess->mEventManager->RemoveAllHandles();

	fq::game_module::ObjectPool::Finalize();

	// Window 종료
	mGameProcess->mWindowSystem->Finalize();
}

void fq::game_engine::EditorEngine::RenderEditorWinodw()
{
	mEditor->mLightProbeWindow->Render();
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
	mEditor->mNavMeshWindow->Render();
	mEditor->mArticulationHierarchy->Render();
	mEditor->mArticulationInspector->Render();
	mEditor->mExportWindow->Render();
	mEditor->mImportWindow->Render();
	mEditor->mMaterialWindow->Render();
	mEditor->mLightmapWindow->Render();
	mEditor->mGameVariableWindow->Render();
	mEditor->mRenderingDebugWindow->Render();
	mEditor->mClothEditorWindow->Render();
	mEditor->mChangeWindow->Render();

	// 기즈모 세팅 이유로 항상 마지막에 랜더링합니다  
	mEditor->mGamePlayWindow->Render();
}

void fq::game_engine::EditorEngine::InitializeEditor()
{
	// Editor InputManager 초기화
	mEditor->mInputManager->Initialize(mGameProcess->mWindowSystem->GetHWND());

	// System 초기화
	mEditor->mImGuiSystem->Initialize(mGameProcess->mWindowSystem->GetHWND()
		, mGameProcess->mGraphics->GetDivice(), mGameProcess->mGraphics->GetDeviceContext());
	mEditor->mCommandSystem->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mImageSystem->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mModelSystem->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mDebugSystem->Initialize(mGameProcess.get());

	// Window 초기화
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
	mEditor->mNavMeshWindow->Initialize(mGameProcess.get());
	mEditor->mArticulationHierarchy->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mArticulationInspector->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mLightProbeWindow->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mExportWindow->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mImportWindow->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mMaterialWindow->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mLightmapWindow->Initialize(mGameProcess.get());
	mEditor->mGameVariableWindow->Initialize();
	mEditor->mRenderingDebugWindow->Initialize(mGameProcess.get());
	mEditor->mClothEditorWindow->Initialize(mGameProcess.get(), mEditor.get());
	mEditor->mChangeWindow->Initialize(mGameProcess.get(), mEditor.get());

	// Editor Setting
	EditorHelper::LoadEditorSetting(mEditor.get());
}

void fq::game_engine::EditorEngine::UpdateEditor(float dt)
{
	mEditor->mInputManager->Update(dt);

	mEditor->mGamePlayWindow->UpdateCamera(dt);
	mEditor->mGamePlayWindow->UpdateParticle(dt);

	mEditor->mGamePlayWindow->ExcutShortcut();
	mEditor->mMainMenuBar->ExcuteShortcut();
	mEditor->mCommandSystem->ExcuteShortcut();
	mEditor->mHierarchy->ExcuteShortcut();
	mEditor->mPhysicsAnimatorWindow->UpdateAnimation(dt);
}

