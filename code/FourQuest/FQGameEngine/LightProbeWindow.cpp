#include "LightProbeWindow.h"

#include "LightProbeSystem.h"
#include "GameProcess.h"

#include <imgui.h>
#include "imgui_stdlib.h"

fq::game_engine::LightProbeWindow::LightProbeWindow()
	:mbIsOpen(false),
	mSaveFileName("LightProbe")
{
}

fq::game_engine::LightProbeWindow::~LightProbeWindow()
{
}

void fq::game_engine::LightProbeWindow::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mLightProbeSystem = game->mLightProbeSystem.get();
}

void fq::game_engine::LightProbeWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("LightProbe", &mbIsOpen))
	{
		beginButtons();
	}
	ImGui::End();
}

void fq::game_engine::LightProbeWindow::beginButtons()
{
	if (ImGui::Button("Add", ImVec2{ 133,25 }))
	{
		mLightProbeSystem->AddLightProbe({ 0, 0, 0 });
	}
	if (ImGui::Button("Bake", ImVec2{ 133,25 }))
	{
		//mLightProbeSystem->SaveNavMesh(mSaveFileName);
	}
	ImGui::InputText(".Ltp", &mSaveFileName);
	if (ImGui::Button("Save", ImVec2{ 133,25 }))
	{
		//mLightProbeSystem->SaveNavMesh(mSaveFileName);
	}
	if (ImGui::Button("Load", ImVec2{ 133,25 }))
	{
		//mLightProbeSystem->LoadNavMesh(mSaveFileName);
	}
}
