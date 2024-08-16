#include "NavigationMeshWindow.h"

#include <imgui.h>

#include "PathFindingSystem.h"
#include "GameProcess.h"
#include "../FQGameModule/SceneManager.h"
#include "NavigationMesh.h"

#include "imgui_stdlib.h"

fq::game_engine::NavigationMeshWindow::NavigationMeshWindow()
	:mbIsOpen(false),
	mSaveFileName("NavMesh")
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
	// IS TRUE 인 STATIC MESH OBJECT만 굽기
	if (ImGui::Button("Build", ImVec2{ 133,25 }))
	{
		fq::game_module::Scene* scene = mGameProcess->mSceneManager->GetCurrentScene();
		mPathFindingSystem->BuildNavigationMesh(scene, false);
	}
	// IS 여부에 상관없이 다 굽기
	if (ImGui::Button("BuildAll", ImVec2{ 133,25 }))
	{
		fq::game_module::Scene* scene = mGameProcess->mSceneManager->GetCurrentScene();
		mPathFindingSystem->BuildNavigationMesh(scene, true);
	}
	// 모두 다 IS ON 하기
	if (ImGui::Button("AllIsOnUsedNavMesh", ImVec2{ 133,25 }))
	{
		fq::game_module::Scene* scene = mGameProcess->mSceneManager->GetCurrentScene();
		mPathFindingSystem->IsOnAllMeshUsedNavMesh(scene);
	}
	ImGui::InputText("Object Name", &mObjectName);
	// 특정 이름을 가진 오브젝트 IS OFF 하기 
	if (ImGui::Button("IsOffToName", ImVec2{ 133,25 }))
	{
		fq::game_module::Scene* scene = mGameProcess->mSceneManager->GetCurrentScene();
		mPathFindingSystem->IsOffMeshToName(scene, mObjectName);
	}

	ImGui::InputText(".Nav", &mSaveFileName);
	if (ImGui::Button("Save", ImVec2{ 133,25 }))
	{
		mPathFindingSystem->SaveNavMesh(mSaveFileName);
	}
	if (ImGui::Button("Load", ImVec2{ 133,25 }))
	{
		mPathFindingSystem->LoadNavMesh(mSaveFileName);
	}
}

/*
int maxCrowdNumber{ 256 }; // 길찾기 주체들의 최대 개체수
float maxAgentRadius{ 0.6 }; // 길찾기 주체들의 최대 충돌반경
float walkableSlopeAngle{ 30 }; // 오를수 있는 경사
float walkableClimb{ 0.2 }; // 오를 수 있는 단차
float walkableHeight{ 0.3 }; // 천장의 최소 높이
float divisionSizeXZ{ 0.3 }; // x축,z축 공간 분할의 단위, 단위가 작을수록 판정이 더 세밀해지지만, 네비게이션 빌드와 길찾기 시스템의 부하가 늘게 된다.
float divisionSizeY{ 0.2 }; // y축 공간 분할의 단위, 단위가 작을수록 판정이 더 세밀해지지만, 네비게이션 빌드와 길찾기 시스템의 부하가 늘게 된다.*/

void fq::game_engine::NavigationMeshWindow::beginNavigationMeshDataInput()
{
	BuildSettings* buildSettings = mPathFindingSystem->GetBuildingSettrings();

	//if (ImGui::InputFloat())
	//ImGui::SetNextItemWidth(100.f);
	if (ImGui::InputInt("MaxMonster", &buildSettings->maxCrowdNumber))
	{
	}
	if (ImGui::InputFloat("MaxCollisionRadius", &buildSettings->maxAgentRadius))
	{
	}
	if (ImGui::InputFloat("WalkableSlopeAngle", &buildSettings->walkableSlopeAngle))
	{
	}
	if (ImGui::InputFloat("WalkableClimb", &buildSettings->walkableClimb))
	{
	}
	if (ImGui::InputFloat("WalkableMinHeight", &buildSettings->walkableHeight))
	{
	}
	if (ImGui::InputFloat("DivisionSizeXZ", &buildSettings->divisionSizeXZ))
	{
	}
	if (ImGui::InputFloat("DivisionSizeY", &buildSettings->divisionSizeY))
	{
	}
}

void fq::game_engine::NavigationMeshWindow::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mPathFindingSystem = game->mPathFindgingSystem.get();
}
