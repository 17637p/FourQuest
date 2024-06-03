#include "Setting.h"

#include <imgui.h>

void fq::game_engine::Setting::Initialize(GameProcess* game, EditorProcess* editor)
{

}

void fq::game_engine::Setting::Render()
{
	if (!mbIsOpen) return;

	if(ImGui::Begin("Setting", &mbIsOpen))
	{
		beginChild_SnapSetting();

	}

	ImGui::End();
}

void fq::game_engine::Setting::beginChild_SnapSetting()
{
	if (ImGui::BeginChild("Snap"), ImVec2(0,0), ImGuiChildFlags_Border)
	{
		ImGui::Checkbox("UseSnap", &mbUseSnap);
		ImGui::InputFloat3("Snap", mSnap);
	}
	ImGui::EndChild();

}

fq::game_engine::Setting::Setting()
	:mbUseSnap(false)
	,mbIsOpen(false)
	,mSnap{}
{

}
