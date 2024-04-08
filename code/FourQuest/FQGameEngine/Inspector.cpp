#include "Inspector.h"

#include <imgui.h>


fq::game_engine::Inspector::Inspector()
{

}

fq::game_engine::Inspector::~Inspector()
{

}

void fq::game_engine::Inspector::Render()
{
	if (ImGui::Begin("Inspector"))
	{
		


		ImGui::End();
	}
}
