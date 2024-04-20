#pragma once

#include <memory>

namespace fq::game_module
{
	class InputManager;
}

namespace fq::game_engine
{
	// window
	class MenuBar;
	class Hierarchy;
	class Inspector;
	class FileDialog;
	class LogWindow;
	class GamePlayWindow;

	// system
	class CommandSystem;
	class PrefabSystem;

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

		std::unique_ptr<CommandSystem> mCommandSystem;
		std::unique_ptr<PrefabSystem> mPrefabSystem;

		std::unique_ptr<fq::game_module::InputManager> mInputManager;
	};


}