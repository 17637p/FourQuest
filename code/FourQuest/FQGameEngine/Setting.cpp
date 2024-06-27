#include "Setting.h"

#include <imgui.h>

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"

fq::game_engine::Setting::Setting()
	:mbUseSnap(false)
	, mbIsOpen(false)
	, mbUseGrayScale(false)
	, mSnap{}
	, mMode(ImGuizmo::WORLD)
{

}


void fq::game_engine::Setting::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
}

void fq::game_engine::Setting::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("Setting", &mbIsOpen))
	{
		beginChild_GizumoSetting();
		beginChild_GraphicsSetting();
	}

	ImGui::End();
}

void fq::game_engine::Setting::beginChild_GizumoSetting()
{

	if (ImGui::CollapsingHeader("Gizumo"))
	{
		if (ImGui::BeginChild("Gizumo"), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY)
		{
			ImGui::Checkbox("UseSnap", &mbUseSnap);
			ImGui::InputFloat3("Snap", mSnap);

			std::string currentMode = (mMode == ImGuizmo::WORLD) ? "World" : "Local";

			if (ImGui::BeginCombo("Mode", currentMode.c_str()))
			{
				if (ImGui::Selectable("World"))
				{
					mMode = ImGuizmo::WORLD;
				}
				if (ImGui::Selectable("Local"))
				{
					mMode = ImGuizmo::LOCAL;
				}

				ImGui::EndCombo();
			}
		}
		ImGui::EndChild();
	}
}


void fq::game_engine::Setting::beginChild_GraphicsSetting()
{
	if (ImGui::CollapsingHeader("Graphics"))
	{
		if (ImGui::BeginChild("Graphics"), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY)
		{
			if (ImGui::Checkbox("UseGrayScale", &mbUseGrayScale))
			{
				auto materials = mGameProcess->mGraphics->GetMaterials();

				for (auto& material : materials)
				{
					auto materialInfo = material->GetInfo();

					materialInfo.bUseBaseColor = !mbUseGrayScale;
					materialInfo.bUseMetalness = !mbUseGrayScale;
					materialInfo.bUseNormalness = !mbUseGrayScale;
					materialInfo.bUseRoughness = !mbUseGrayScale;
					materialInfo.BaseColor = { 1.f,1.f,1.f,1.f };

					material->SetInfo(materialInfo);
				}

			}
		}
		ImGui::EndChild();
	}



}

ImGuizmo::MODE fq::game_engine::Setting::GetMode()
{
	return mMode;
}

