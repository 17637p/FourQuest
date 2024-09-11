#include "GameVariableWindow.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include "imgui_stdlib.h"

#include "../FQClient/PlayerSoulVariable.h"
#include "../FQClient/MonsterVariable.h"
#include "../FQClient/GameVaribleHelper.h"

fq::game_engine::GameVariableWindow::GameVariableWindow()
	:mbIsOpen(false)
{

}

fq::game_engine::GameVariableWindow::~GameVariableWindow()
{

}

void fq::game_engine::GameVariableWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("GameVariable", &mbIsOpen))
	{
		for (auto& type : mVariableTypes)
		{
			beginVariable(type);
		}
	}
	ImGui::End();
}

void fq::game_engine::GameVariableWindow::beginVariable(entt::meta_type type)
{
	auto name = fq::reflect::GetName(type);

	if (ImGui::CollapsingHeader(name.c_str()))
	{
		if (ImGui::BeginChild(name.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY))
		{
			auto  variable = type.construct();

			for (auto [id, metaData] : type.data())
			{
				auto memberType = metaData.type();

				if (memberType == entt::resolve<float>())
				{
					float val = metaData.get(variable).cast<float>();
					std::string memberName = fq::reflect::GetName(metaData);

					if (ImGui::InputFloat(memberName.c_str(), &val))
					{
						metaData.set(variable, val);
					}
				}
				else if (memberType == entt::resolve<int>())
				{
					int val = metaData.get(variable).cast<int>();
					std::string memberName = fq::reflect::GetName(metaData);

					if (ImGui::InputInt(memberName.c_str(), &val))
					{
						metaData.set(variable, val);
					}
				}
				else if (memberType == entt::resolve<bool>())
				{
					bool val = metaData.get(variable).cast<bool>();
					std::string memberName = fq::reflect::GetName(metaData);

					if (ImGui::Checkbox(memberName.c_str(), &val))
					{
						metaData.set(variable, val);
					}
				}
				else if (memberType == entt::resolve<std::string>())
				{
					std::string val = metaData.get(variable).cast<std::string>();
					std::string memberName = fq::reflect::GetName(metaData);

					if (ImGui::InputText(memberName.c_str(), &val))
					{
						metaData.set(variable, val);
					}
				}
			}
		}
		ImGui::EndChild();
	}
}

void fq::game_engine::GameVariableWindow::Initialize()
{
	// 게임 변수 타입을 가져옵니다 
	for (const auto& [id, type] : entt::resolve())
	{
		for (auto [bsaeID, baseType] : type.base())
		{
			if (baseType == entt::resolve<client::IGameVariable>())
			{
				mVariableTypes.push_back(type);
			}
		}
	}
}
