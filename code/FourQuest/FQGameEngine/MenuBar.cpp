#include "MenuBar.h"

#include <imgui.h>

fq::game_engine::MenuBar::MenuBar()
	:mGameProcess(nullptr)
	,mEditorProcess(nullptr)
{

}

fq::game_engine::MenuBar::~MenuBar()
{

}

void fq::game_engine::MenuBar::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
}

void fq::game_engine::MenuBar::Render()
{
	if (ImGui::BeginMainMenuBar())
	{


	}
	ImGui::EndMainMenuBar();

}


