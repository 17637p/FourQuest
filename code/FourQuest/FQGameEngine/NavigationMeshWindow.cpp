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
		//auto matrix = mPhysicsSystem->GetCollisionMatrix();
		//matrix.Reset();
		//mPhysicsSystem->SetCollisionMatrix(matrix);
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
	NavigationMeshBuilder::BuildSettings buildSettings;

	buildSettings.maxCrowdNumber;
	buildSettings.maxAgentRadius;
	buildSettings.walkableSlopeAngle;
	buildSettings.walkableClimb;
	buildSettings.walkableHeight;
	buildSettings.divisionSizeXZ;
	buildSettings.divisionSizeY;

	//if (ImGui::InputFloat())
	ImGui::SetNextItemWidth(100.f);
	int val = buildSettings.maxCrowdNumber;
	if (ImGui::InputInt(std::to_string(buildSettings.maxCrowdNumber).c_str(),
		&buildSettings.maxCrowdNumber))
	{
		//mSelectController->SetParameter(id, val);
	}
}
