#include "Setting.h"

#include <imgui.h>

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"

void fq::game_engine::Setting::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
}

void fq::game_engine::Setting::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("Setting", &mbIsOpen))
	{
		beginChild_SnapSetting();
		beginChild_GraphicsSetting();
	}

	ImGui::End();
}

void fq::game_engine::Setting::beginChild_SnapSetting()
{

	if (ImGui::CollapsingHeader("Snap"))
	{

		if (ImGui::BeginChild("Snap"), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY)
		{
			ImGui::Checkbox("UseSnap", &mbUseSnap);
			ImGui::InputFloat3("Snap", mSnap);
		}
		ImGui::EndChild();
	}

}

fq::game_engine::Setting::Setting()
	:mbUseSnap(false)
	, mbIsOpen(false)
	, mbUseGrayScale(false)
	, mSnap{}
{

}

void fq::game_engine::Setting::beginChild_GraphicsSetting()
{
	if (ImGui::CollapsingHeader("Graphics"))
	{
		if (ImGui::BeginChild("Graphics"), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY)
		{
			if (ImGui::Checkbox("UseGrayScale", &mbUseGrayScale))
			{
				auto materials =  mGameProcess->mGraphics->GetMaterials();

				for (auto& material : materials)
				{
					fq::graphics::MaterialTextureUseFlag flag{};
					flag.bUseBaseColor = !mbUseGrayScale;
					flag.bUseMetalness = !mbUseGrayScale;
					flag.bUseNormalness = !mbUseGrayScale;
					flag.bUseRoughness = !mbUseGrayScale;

					material->SetTextureUseFlag(flag);
				
					if (mbUseGrayScale)
					{
						fq::common::Material mat;
						mat.MaterialDesc.BaseColor = { 1.f,1.f,1.f,1.f };
						material->SetMaterialData(mat);
					}
				}

			}
		}
		ImGui::EndChild();
	}



}

