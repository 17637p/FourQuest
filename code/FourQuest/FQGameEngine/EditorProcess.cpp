#include "EditorProcess.h"

#include "CommandSystem.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "FileDialog.h"
#include "DebugViewer.h"


fq::game_engine::EditorProcess::EditorProcess()
	: mHierarchy(std::make_unique<Hierarchy>())
	, mInspector(std::make_unique<Inspector>())
	, mFileDialog(std::make_unique<FileDialog>())
	, mDeubgViewer(std::make_unique<DebugViewer>())
	, mCommandSystem(std::make_unique<CommandSystem>())
{

}

fq::game_engine::EditorProcess::~EditorProcess()
{

}
