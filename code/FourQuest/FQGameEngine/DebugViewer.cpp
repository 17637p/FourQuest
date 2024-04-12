#include "DebugViewer.h"

#include <imgui.h>

fq::game_engine::DebugViewer::DebugViewer()
{

}

fq::game_engine::DebugViewer::~DebugViewer()
{

}

void fq::game_engine::DebugViewer::Render()
{
	if (ImGui::Begin("DebugViewer"))
	{


		ImGui::End();
	}
}
