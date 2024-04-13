#include "EditorProcess.h"

#include "CommandSystem.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "FileDialog.h"
#include "LogWindow.h"
#include "GamePlayWindow.h"

#include "../FQGameModule/InputManager.h"

fq::game_engine::EditorProcess::EditorProcess()
	: mHierarchy(std::make_unique<Hierarchy>())
	, mInspector(std::make_unique<Inspector>())
	, mFileDialog(std::make_unique<FileDialog>())
	, mLogWindow(std::make_unique<LogWindow>())
	, mCommandSystem(std::make_unique<CommandSystem>())
	, mInputManager(std::make_unique<fq::game_module::InputManager>())
	, mGamePlayWindow(std::make_unique<GamePlayWindow>())
{

}

fq::game_engine::EditorProcess::~EditorProcess()
{

}
