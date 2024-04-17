#include "LogWindow.h"

#include <imgui.h>

fq::game_engine::LogWindow::LogWindow()
{

}

fq::game_engine::LogWindow::~LogWindow()
{

}

void fq::game_engine::LogWindow::Render()
{
	if (ImGui::Begin("Log"))
	{


	}
	ImGui::End();
}
