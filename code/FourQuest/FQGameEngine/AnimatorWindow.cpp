#include "AnimatorWindow.h"

#include <imgui-node-editor/imgui_node_editor.h>

#include "imgui_stdlib.h"
#include "EditorProcess.h"
#include "GameProcess.h"

namespace ed = ax::NodeEditor;

fq::game_engine::AnimatorWindow::AnimatorWindow()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mbIsOpen(false)
	, mContext(nullptr)
{

}

fq::game_engine::AnimatorWindow::~AnimatorWindow()
{

}

void fq::game_engine::AnimatorWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

	// Context 생성
	ed::Config config;
	config.SettingsFile = "Simple.json";
	mContext = ed::CreateEditor(&config);

	// 임시로 컨트롤러 생성
	mSelectController = std::make_shared<fq::game_module::AnimatorController>();
}

void fq::game_engine::AnimatorWindow::Render()
{
	if (!mbIsOpen) return;

	auto& io = ImGui::GetIO();

	if (ImGui::Begin("Animator"))
	{
		beginChild_ParameterWindow();

		ImGui::SameLine();

		beginChild_Node();
	}
	ImGui::End();
}

void fq::game_engine::AnimatorWindow::Finalize()
{
	ed::DestroyEditor(mContext);
}

void fq::game_engine::AnimatorWindow::beginChild_ParameterWindow()
{
	if (ImGui::BeginChild("Parameter", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX))
	{
		// Add Parametr
		beginCombo_AddParameter();

		// Parameters
		auto parameters = mSelectController->GetParameterPack();

		std::string originName;
		std::string changedName;

		for (auto& [id, parameter] : parameters)
		{
			// Name
			std::string parametetName = id;
			std::string label = "##" + parametetName;

			ImGui::SetNextItemWidth(100.f);
			ImGui::InputText(label.c_str(), &parametetName);

			// Name이 변경된 경우
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				originName = id;
				changedName = parametetName;
			}

			ImGui::SameLine();

			// Setter
			std::string parameterSetLabel = "##" + parametetName + parametetName;

			if (parameter.type() == entt::resolve<int>())
			{
				int val = parameter.cast<int>();
				if (ImGui::InputInt(parameterSetLabel.c_str(), &val))
				{
					mSelectController->SetParameter(id, val);
				}
			}
			else if (parameter.type() == entt::resolve<float>())
			{
				float val = parameter.cast<float>();
				if (ImGui::InputFloat(parameterSetLabel.c_str(), &val))
				{
					mSelectController->SetParameter(id, val);
				}
			}
			else if (parameter.type() == entt::resolve<bool>())
			{
				bool val = parameter.cast<bool>();
				if (ImGui::Checkbox(parameterSetLabel.c_str(), &val))
				{
					mSelectController->SetParameter(id, val);
				}
			}
			else if (parameter.type() == entt::resolve<char>())
			{
				bool val = static_cast<bool>(parameter.cast<char>());
				if (ImGui::RadioButton(parameterSetLabel.c_str(), val))
				{
					val = !val;
					mSelectController->SetParameter(id, static_cast<char>(val));
				}
			}
			else assert(nullptr);
		}

		// 이름 변경 처리
		if (!originName.empty())
		{
			auto parameter = mSelectController->GetParameter(originName);
			mSelectController->EraseParameter(originName);
			mSelectController->AddParameter(changedName, parameter);
		}

	}
	ImGui::EndChild();

}

void fq::game_engine::AnimatorWindow::beginChild_Node()
{
	if (ImGui::BeginChild("Node", ImVec2(0, 0)))
	{
		ImGui::Separator();
		ed::SetCurrentEditor(mContext);
		ed::Begin("My Editor", ImVec2(0.0, 0.0f));
		int uniqueId = 1;
		// Start drawing nodes.
		ed::BeginNode(uniqueId++);
		ImGui::Text("Node A");
		ed::BeginPin(uniqueId++, ed::PinKind::Input);
		ImGui::Text("-> In");
		ed::EndPin();
		ImGui::SameLine();
		ed::BeginPin(uniqueId++, ed::PinKind::Output);
		ImGui::Text("Out ->");
		ed::EndPin();
		ed::EndNode();
		ed::End();
		ed::SetCurrentEditor(nullptr);
	}
	ImGui::EndChild();

}

void fq::game_engine::AnimatorWindow::beginCombo_AddParameter()
{
	ImGui::SetNextItemWidth(100.f);

	if (ImGui::BeginCombo("Add Parameter", "int"))
	{
		if (ImGui::Selectable("int##button"))
		{
			mSelectController->AddParameter("new_int", 0);
		}
		if (ImGui::Selectable("float##button"))
		{
			mSelectController->AddParameter("new_float", 0.f);
		}
		if (ImGui::Selectable("bool##button"))
		{
			mSelectController->AddParameter("new_bool", false);
		}
		if (ImGui::Selectable("trigger##button"))
		{
			mSelectController->AddParameter("new_trigger"
				, fq::game_module::AnimatorController::OffTrigger);
		}
		ImGui::EndCombo();
	}


}
