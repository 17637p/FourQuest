#include "GamePlayWindow.h"

#include <imgui.h>

#include "EditorProcess.h"
#include "GameProcess.h"

fq::game_engine::GamePlayWindow::GamePlayWindow()
	:mGameProcess(nullptr)
	,mEditorProcess(nullptr)
{

}

fq::game_engine::GamePlayWindow::~GamePlayWindow()
{

}

void fq::game_engine::GamePlayWindow::Render()
{
	if (ImGui::Begin("GamePlay"))
	{


	}
	ImGui::End();
}

void fq::game_engine::GamePlayWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
}

