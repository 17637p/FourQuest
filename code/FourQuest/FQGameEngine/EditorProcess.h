#pragma once

#include <memory>

//input
#include "../FQGameModule/InputManager.h"

// window
#include "Hierarchy.h"
#include "Inspector.h"
#include "FileDialog.h"
#include "LogWindow.h"
#include "GamePlayWindow.h"
#include "MainMenuBar.h"
#include "CollisionMatrixWindow.h"
#include "PhysicsAnimatorWindow.h"
#include "SkyBoxWindow.h"
#include "AnimatorWindow.h"
#include "Setting.h"
#include "NavigationMeshWindow.h"

// system
#include "ImGuiSystem.h"
#include "CommandSystem.h"
#include "PrefabSystem.h"
#include "ModelSystem.h"
#include "DebugSystem.h"

namespace fq::game_engine
{
	/// <summary>
	/// 에디터가 관리하는 클래스의 집합
	/// </summary>
	class EditorProcess
	{
	public:
		EditorProcess();
		~EditorProcess();

		// Open
		std::unique_ptr<Hierarchy>  mHierarchy;
		std::unique_ptr<Inspector>	mInspector;
		std::unique_ptr<FileDialog> mFileDialog;
		std::unique_ptr<LogWindow> mLogWindow;
		std::unique_ptr<GamePlayWindow> mGamePlayWindow;
		std::unique_ptr<MainMenuBar> mMainMenuBar;
		std::unique_ptr<PhysicsAnimatorWindow> mPhysicsAnimatorWindow;

		// Close
		std::unique_ptr<AnimatorWindow> mAnimatorWindow;
		std::unique_ptr<CollisionMatrixWindow> mCollisionMatrixWindow;
		std::unique_ptr<SkyBoxWindow> mSkyBoxWindow;
		std::unique_ptr<Setting> mSettingWindow;
		std::unique_ptr<NavigationMeshWindow> mNavMeshWindow;

		// System
		std::unique_ptr<ImGuiSystem> mImGuiSystem;
		std::unique_ptr<CommandSystem> mCommandSystem;
		std::unique_ptr<PrefabSystem> mPrefabSystem;
		std::unique_ptr<ModelSystem> mModelSystem;
		std::unique_ptr<DebugSystem> mDebugSystem;

		std::unique_ptr<fq::game_module::InputManager> mInputManager;
	};


}