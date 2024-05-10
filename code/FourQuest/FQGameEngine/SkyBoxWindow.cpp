#include "SkyBoxWindow.h"

#include <imgui.h>

#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"

fq::game_engine::SkyBoxWindow::SkyBoxWindow()
	:mGraphicsEngine(nullptr)
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
	}

	ImGui::End();
}

