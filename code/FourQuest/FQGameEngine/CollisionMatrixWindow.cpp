#include "CollisionMatrixWindow.h"

#include <imgui.h>
#include "ImGuiColor.h"

#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"
#include "PhysicsSystem.h"

fq::game_engine::CollisionMatrixWindow::CollisionMatrixWindow()
	:mGameProcess(nullptr)
	, mPhysicsSystem(nullptr)
	, mbIsOpen(false)
{}

fq::game_engine::CollisionMatrixWindow::~CollisionMatrixWindow()
{}

void fq::game_engine::CollisionMatrixWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("CollisionMatrix",&mbIsOpen))
	{
		beginButton_Reset();
		beginCollisionMatrix();
	}
	ImGui::End();
}

void fq::game_engine::CollisionMatrixWindow::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mPhysicsSystem = game->mPhysicsSystem.get();
}

void fq::game_engine::CollisionMatrixWindow::beginButton_Reset()
{
	if (ImGui::Button("Reset", ImVec2{ 133,25 }))
	{
		auto matrix = mPhysicsSystem->GetCollisionMatrix();
		matrix.Reset();
		mPhysicsSystem->SetCollisionMatrix(matrix);
	}
}

void fq::game_engine::CollisionMatrixWindow::beginCollisionMatrix()
{
	for (int i = 0; i < 16; ++i)
	{
		ImGui::SameLine();
		std::string row = std::to_string(i);
		ImGui::Button(row.c_str(), ImVec2(25, 25));
	}


	auto matrix = mPhysicsSystem->GetCollisionMatrix();

	auto type = entt::resolve<fq::game_module::ETag>();
	int index = 0;
	for (auto [id, type] : type.data())
	{
		auto name = fq::reflect::GetName(type);

		ImGui::Button(name.c_str(), ImVec2(100, 25));
		ImGui::SameLine();

		std::string sIndex = std::to_string(index) + "##col" + std::to_string(index);
		ImGui::Button(sIndex.c_str(), ImVec2(25, 25));
		for (int j = 0; j < 16; ++j)
		{
			bool isCheck = matrix.data[index][j];

			std::string buttonName;
			if (isCheck)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGuiColor::RED);
				buttonName = "O";
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGuiColor::DARK_GRAY);
				buttonName = "X";
			}
			buttonName += "##" + name + std::to_string(index) + std::to_string(j);
			ImGui::SameLine();
			if (ImGui::Button(buttonName.c_str(), ImVec2(25, 25)))
			{
				matrix.data[index][j].flip();
				matrix.data[j][index] =matrix.data[index][j];
			}

			ImGui::PopStyleColor();
		}
		++index;
	}

	mPhysicsSystem->SetCollisionMatrix(matrix);
}
