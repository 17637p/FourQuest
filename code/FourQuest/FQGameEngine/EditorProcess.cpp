#include "EditorProcess.h"

#include "CommandSystem.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "FileDialog.h"
#include "DebugViewer.h"

#include "../FQGameModule/InputManager.h"

fq::game_engine::EditorProcess::EditorProcess()
	: mHierarchy(std::make_unique<Hierarchy>())
	, mInspector(std::make_unique<Inspector>())
	, mFileDialog(std::make_unique<FileDialog>())
	, mDeubgViewer(std::make_unique<DebugViewer>())
	, mCommandSystem(std::make_unique<CommandSystem>())
	, mInputManager(std::make_unique<fq::game_module::InputManager>())
{

}

fq::game_engine::EditorProcess::~EditorProcess()
{

}
