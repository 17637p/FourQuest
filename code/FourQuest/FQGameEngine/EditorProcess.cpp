#include "EditorProcess.h"


fq::game_engine::EditorProcess::EditorProcess()
	: mHierarchy(std::make_unique<Hierarchy>())
	, mInspector(std::make_unique<Inspector>())
	, mFileDialog(std::make_unique<FileDialog>())
	, mLogWindow(std::make_unique<LogWindow>())
	, mCommandSystem(std::make_unique<CommandSystem>())
	, mInputManager(std::make_unique<fq::game_module::InputManager>())
	, mGamePlayWindow(std::make_unique<GamePlayWindow>())
	, mSkyBoxWindow(std::make_unique<SkyBoxWindow>())
	, mAnimatorWindow(std::make_unique<AnimatorWindow>())
	, mSettingWindow(std::make_unique<Setting>())
	, mMainMenuBar(std::make_unique<MainMenuBar>())
	, mPrefabSystem(std::make_unique<PrefabSystem>())
	, mImGuiSystem(std::make_unique<ImGuiSystem>())
	, mModelSystem(std::make_unique <ModelSystem>())
	, mDebugSystem(std::make_unique <DebugSystem>())
	, mCollisionMatrixWindow(std::make_unique<CollisionMatrixWindow>())
	, mPhysicsAnimatorWindow(std::make_unique<PhysicsAnimatorWindow>())
	, mNavMeshWindow(std::make_unique<NavigationMeshWindow>())
{}

fq::game_engine::EditorProcess::~EditorProcess()
{}
