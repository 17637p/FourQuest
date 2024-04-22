#include "EditorProcess.h"


fq::game_engine::EditorProcess::EditorProcess()
	: mHierarchy(std::make_unique<Hierarchy>())
	, mInspector(std::make_unique<Inspector>())
	, mFileDialog(std::make_unique<FileDialog>())
	, mLogWindow(std::make_unique<LogWindow>())
	, mCommandSystem(std::make_unique<CommandSystem>())
	, mInputManager(std::make_unique<fq::game_module::InputManager>())
	, mGamePlayWindow(std::make_unique<GamePlayWindow>())
	, mMainMenuBar(std::make_unique<MainMenuBar>())
	, mPrefabSystem(std::make_unique<PrefabSystem>())
	, mImGuiSystem(std::make_unique<ImGuiSystem>())
{}

fq::game_engine::EditorProcess::~EditorProcess()
{}
