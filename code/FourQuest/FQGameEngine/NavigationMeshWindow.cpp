#include "NavigationMeshWindow.h"

#include <imgui.h>

#include "PathFindingSystem.h"
#include "GameProcess.h"

fq::game_engine::NavigationMeshWindow::NavigationMeshWindow()
	:mbIsOpen(false)
{
}

fq::game_engine::NavigationMeshWindow::~NavigationMeshWindow()
{
}

void fq::game_engine::NavigationMeshWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("NavigationMesh", &mbIsOpen))
	{
		beginNavigationMeshDataInput();
		beginButtonBuild();
	}
	ImGui::End();
}

void fq::game_engine::NavigationMeshWindow::beginButtonBuild()
{
	if (ImGui::Button("Build", ImVec2{ 133,25 }))
	{
		fq::game_module::Scene* scene = mGameProcess->mSceneManager->GetCurrentScene();
		mPathFindingSystem->BuildNavigationMesh(scene);
	}
}

/*
int maxCrowdNumber{ 256 }; // ��ã�� ��ü���� �ִ� ��ü��
float maxAgentRadius{ 0.6 }; // ��ã�� ��ü���� �ִ� �浹�ݰ�
float walkableSlopeAngle{ 30 }; // ������ �ִ� ���
float walkableClimb{ 0.2 }; // ���� �� �ִ� ����
float walkableHeight{ 0.3 }; // õ���� �ּ� ����
float divisionSizeXZ{ 0.3 }; // x��,z�� ���� ������ ����, ������ �������� ������ �� ������������, �׺���̼� ����� ��ã�� �ý����� ���ϰ� �ð� �ȴ�.
float divisionSizeY{ 0.2 }; // y�� ���� ������ ����, ������ �������� ������ �� ������������, �׺���̼� ����� ��ã�� �ý����� ���ϰ� �ð� �ȴ�.*/

void fq::game_engine::NavigationMeshWindow::beginNavigationMeshDataInput()
{
	NavigationMeshBuilder::BuildSettings& buildSettings = mPathFindingSystem->GetBuildingSettrings();

	//if (ImGui::InputFloat())
	//ImGui::SetNextItemWidth(100.f);
	if (ImGui::InputInt("MaxMonster", &buildSettings.maxCrowdNumber))
	{
	}
	if (ImGui::InputFloat("MaxCollisionRadius", &buildSettings.maxAgentRadius))
	{
	}
	if (ImGui::InputFloat("WalkableSlopeAngle", &buildSettings.walkableSlopeAngle))
	{
	}
	if (ImGui::InputFloat("WalkableClimb", &buildSettings.walkableClimb))
	{
	}
	if (ImGui::InputFloat("WalkableMinHeight", &buildSettings.walkableHeight))
	{
	}
	if (ImGui::InputFloat("DivisionSizeXZ", &buildSettings.divisionSizeXZ))
	{
	}
	if (ImGui::InputFloat("DivisionSizeY", &buildSettings.divisionSizeY))
	{
	}
}

void fq::game_engine::NavigationMeshWindow::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mPathFindingSystem = game->mPathFindgingSystem.get();
}
