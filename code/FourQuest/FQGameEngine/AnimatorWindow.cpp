#include "AnimatorWindow.h"

#include <imgui-node-editor/imgui_node_editor.h>

#include "imgui_stdlib.h"
#include "EditorProcess.h"
#include "GameProcess.h"
#include "ImGuiColor.h"

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
	config.SettingsFile = "resource/internal/animator/Simple.json";
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

		beginChild_NodeEditor();
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
				ImGui::SetNextItemWidth(100.f);
				int val = parameter.cast<int>();
				if (ImGui::InputInt(parameterSetLabel.c_str(), &val))
				{
					mSelectController->SetParameter(id, val);
				}
			}
			else if (parameter.type() == entt::resolve<float>())
			{
				ImGui::SetNextItemWidth(100.f);
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

void fq::game_engine::AnimatorWindow::beginChild_NodeEditor()
{
	if (ImGui::BeginChild("Node", ImVec2(0, 0)))
	{
		ed::SetCurrentEditor(mContext);
		ImGui::Separator();
		ed::Begin("NodeEditor", ImVec2(0.0, 0.0f));


		const auto& stateMap = mSelectController->GetStateMap();
		for (const auto& [stateName, state] : stateMap)
		{
			beginNode_AnimationStateNode(stateName, state);
		}


		//int uniqueId = 1;
		//ed::BeginNode(uniqueId++);//1
		//ImGui::Text("Node A");
		//ed::BeginPin(uniqueId++, ed::PinKind::Input); //2
		//ImGui::Text("-> In");
		//ed::EndPin();
		//ImGui::SameLine();
		//ed::BeginPin(uniqueId++, ed::PinKind::Output); //3
		//ImGui::Text("Out ->");
		//ed::EndPin();
		//ed::EndNode();
		//ed::BeginNode(uniqueId++); //4
		//ImGui::Text("Node A");
		//ed::BeginPin(uniqueId++, ed::PinKind::Input); //5
		//ImGui::Text("-> In");
		//ed::EndPin();
		//ImGui::SameLine();
		//ed::BeginPin(uniqueId++, ed::PinKind::Output); //6
		//ImGui::Text("Out ->");
		//ed::EndPin();
		//ed::EndNode();
		//ed::Link(uniqueId, 2, 6);

		ed::End();
		ed::SetCurrentEditor(nullptr);
		beginPopupContextWindow_NodeEditor();

		ImGui::EndChild();
	}

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

	ImGui::Separator();
}

void fq::game_engine::AnimatorWindow::beginPopupContextWindow_NodeEditor()
{
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create State"))
		{
			mSelectController->CreateStateNode();
		}
		ImGui::EndPopup();
	}

}

void fq::game_engine::AnimatorWindow::beginNode_AnimationStateNode(const std::string& name
	, const fq::game_module::AnimationStateNode& node)
{
	using NodeType = game_module::AnimationStateNode::Type;

	// Node 컬러 
	ImVec4 nodeColor = ImGuiColor::DARK_GRAY;
	if (node.GetType() == NodeType::Entry) nodeColor = ImGuiColor::GREEN;
	else if (node.GetType() == NodeType::Exit) nodeColor = ImGuiColor::RED;
	else if (node.GetType() ==	NodeType::AnyState)	nodeColor = ImGuiColor::SPRING_GREEN;
	else if (node.GetType() == NodeType::State)	nodeColor = ImGuiColor::ORANGE;
	ed::PushStyleColor(ed::StyleColor_NodeBorder, nodeColor);

	auto nodeID = entt::hashed_string(name.c_str()).value();
	ed::BeginNode(nodeID);
	ImGui::Text(name.c_str());

	ed::EndNode();

	ed::PopStyleColor(1);
}

