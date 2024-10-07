#include "ChangeWindow.h"

#include "GameProcess.h"
#include "EditorProcess.h"


void fq::game_engine::ChangeWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
}

void fq::game_engine::ChangeWindow::Render()
{

}
