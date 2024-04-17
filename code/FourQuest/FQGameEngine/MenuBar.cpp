#include "MenuBar.h"

#include <string>
#include <imgui.h>

#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

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


		beginText_FPS();
	}
	ImGui::EndMainMenuBar();

}

void fq::game_engine::MenuBar::beginText_FPS()
{
	std::string fps = "FPS " + std::to_string(mGameProcess->mTimeManager->GetFPS());

	float cursorPosX = ImGui::GetWindowSize().x - ImGui::CalcTextSize(fps.c_str()).x;

	ImGui::SetCursorPosX(cursorPosX);
	ImGui::Text(fps.c_str());
}

