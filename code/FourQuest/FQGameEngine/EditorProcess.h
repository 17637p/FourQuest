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
#include "MenuBar.h"

// system
#include "ImGuiSystem.h"
#include "CommandSystem.h"
#include "PrefabSystem.h"

namespace fq::game_engine
{
	/// <summary>
	/// �����Ͱ� �����ϴ� Ŭ������ ����
	/// </summary>
	class EditorProcess
	{
	public:
		EditorProcess();
		~EditorProcess();

		std::unique_ptr<Hierarchy>  mHierarchy;
		std::unique_ptr<Inspector>	mInspector;
		std::unique_ptr<FileDialog> mFileDialog;
		std::unique_ptr<LogWindow> mLogWindow;
		std::unique_ptr<GamePlayWindow> mGamePlayWindow;
		std::unique_ptr<MenuBar> mMenuBar;
		
		std::unique_ptr<ImGuiSystem> mImGuiSystem;
		std::unique_ptr<CommandSystem> mCommandSystem;
		std::unique_ptr<PrefabSystem> mPrefabSystem;

		std::unique_ptr<fq::game_module::InputManager> mInputManager;
	};


}