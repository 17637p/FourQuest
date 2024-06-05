#include "SkyBoxWindow.h"

#include <imgui.h>

#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"

fq::game_engine::SkyBoxWindow::SkyBoxWindow()
	:mGraphicsEngine(nullptr)
	,mbIsOpen(false)
{}

fq::game_engine::SkyBoxWindow::~SkyBoxWindow()
{

}

void fq::game_engine::SkyBoxWindow::Initialize(GameProcess* game)
{
	mGraphicsEngine = game->mGraphics;
}

void fq::game_engine::SkyBoxWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("SkyBox", &mbIsOpen))
	{
		dragDropTarget_SkyBox();
	}

	ImGui::End();
}

void fq::game_engine::SkyBoxWindow::dragDropTarget_SkyBox()
{
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* path
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			if (path->extension() == ".dds") // ¸ðµ¨ »ý¼º
			{
				mGraphicsEngine->SetSkyBox(*path);
				mGraphicsEngine->SetIBLTexture(L"./resource/example/texture/default01DiffuseHDR.dds",
					L"./resource/example/texture/default01SpecularHDR.dds",
					L"./resource/example/texture/default01Brdf.dds");
			}
		}
	}
}

