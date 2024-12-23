#include "GameVariableWindow.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include "imgui_stdlib.h"
#include <directxtk/SimpleMath.h>

#include "../FQClient/PlayerSoulVariable.h"
#include "../FQClient/LevelVariable.h"
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
				else if (memberType == entt::resolve<DirectX::SimpleMath::Color>())
				{
					auto color = metaData.get(variable).cast<DirectX::SimpleMath::Color>();
					std::string memberName = fq::reflect::GetName(metaData);
					float f[4] = { color.x, color.y ,color.z, color.w };

					if (ImGui::ColorEdit4(memberName.c_str(), f))
					{
						color.x = f[0];
						color.y = f[1];
						color.z = f[2];
						color.w = f[3];

						metaData.set(variable, color);
					}
				}
				beginIsItemHovered_Comment(metaData);
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

void fq::game_engine::GameVariableWindow::beginIsItemHovered_Comment(entt::meta_data data)
{
	auto prop = data.prop(fq::reflect::prop::Comment);

	if (!prop)
	{
		return;
	}

	if (prop.value().type() == entt::resolve<const char*>())
	{
		const char* comment = prop.value().cast<const char*>();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(comment);
		}
	}
	else  if (prop.value().type() == entt::resolve<const char8_t*>())
	{
		const char8_t* comment = prop.value().cast<const char8_t*>();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(reinterpret_cast<const char*>(comment));
		}
	}
}
