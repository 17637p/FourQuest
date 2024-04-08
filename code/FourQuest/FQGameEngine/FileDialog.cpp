#include "FileDialog.h"

#include <imgui.h>

fq::game_engine::FileDialog::FileDialog()
{

}

fq::game_engine::FileDialog::~FileDialog()
{

}

void fq::game_engine::FileDialog::Render()
{
	if (ImGui::Begin("FileDialog"))
	{



		ImGui::End();
	}
}
