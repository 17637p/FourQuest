#include "RenderingDebugWindow.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"

fq::game_engine::RenderingDebugWindow::RenderingDebugWindow()
	:mGraphicsEngine(nullptr)
	, mbIsOpen(false)
	, mGameProcess(nullptr)
{}

fq::game_engine::RenderingDebugWindow::~RenderingDebugWindow()
{

}

void fq::game_engine::RenderingDebugWindow::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mGraphicsEngine = game->mGraphics;
}

void fq::game_engine::RenderingDebugWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("RenderingDebugInfo", &mbIsOpen))
	{
		const auto& debugInfo = mGraphicsEngine->GetDebugInfo();

		ImGui::Text("StaticMeshObjectCount : %d", debugInfo.StaticMeshObjectCount);
		ImGui::Text("SkinnedMeshObjectCount : %d", debugInfo.SkinnedMeshObjectCount);
		ImGui::Text("StaticMeshObjectVertexCount : %d", debugInfo.StaticMeshObjectVertexCount);
		ImGui::Text("SkinnedMeshObjectVertexCount : %d", debugInfo.SkinnedMeshObjectVertexCount);
		ImGui::Text("StaticMeshObjectPolygonCount : %d", debugInfo.StaticMeshObjectPolygonCount);
		ImGui::Text("SkinnedMeshObjectPolygonCount : %d", debugInfo.SkinnedMeshObjectPolygonCount);
		ImGui::Text("CullingStaticMeshObjectCount : %d", debugInfo.CullingStaticMeshObjectCount);
		ImGui::Text("CullingSkinnedMeshObjectCount : %d", debugInfo.CullingSkinnedMeshObjectCount);
		ImGui::Text("CullingStaticMeshObjectVertexCount : %d", debugInfo.CullingStaticMeshObjectVertexCount);
		ImGui::Text("CullingSkinnedMeshObjectVertexCount : %d", debugInfo.CullingSkinnedMeshObjectVertexCount);
		ImGui::Text("CullingStaticMeshObjectPolygonCount : %d", debugInfo.CullingStaticMeshObjectPolygonCount);
		ImGui::Text("CullingSkinnedMeshObjectPolygonCount : %d", debugInfo.CullingSkinnedMeshObjectPolygonCount);
	}

	ImGui::End();
}

