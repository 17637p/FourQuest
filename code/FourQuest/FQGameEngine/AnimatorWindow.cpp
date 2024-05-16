#include "AnimatorWindow.h"

#include <imgui-node-editor/imgui_node_editor.h>

#include "imgui_stdlib.h"
#include "EditorProcess.h"
#include "GameProcess.h"
#include "ImGuiColor.h"
#include "EditorEvent.h"

namespace ed = ax::NodeEditor;

fq::game_engine::AnimatorWindow::AnimatorWindow()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mbIsOpen(true)
	, mContext(nullptr)
	, mMatchPinID{}
{}

fq::game_engine::AnimatorWindow::~AnimatorWindow()
{

}

void fq::game_engine::AnimatorWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mEventManager = mGameProcess->mEventManager.get();

	// Context ����
	ed::Config config;
	config.SettingsFile = "resource/internal/animator/Simple.json";
	mContext = ed::CreateEditor(&config);


	// �ӽ÷� ��Ʈ�ѷ� ����
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

			// Name�� ����� ���
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

		// �̸� ���� ó��
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

		// Node, Pin
		for (const auto& [stateName, state] : stateMap)
		{
			beginNode_AnimationStateNode(stateName, state);
		}

		// Link
		for (const auto& transition : mSelectController->GetTransitions())
		{
			beginLink_AnimationTransition(transition);
		}

		beginCreate();
		beginDelete();
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

	// Node �÷� 
	ImVec4 nodeColor = ImGuiColor::DARK_GRAY;
	if (node.GetType() == NodeType::Entry) nodeColor = ImGuiColor::GREEN;
	else if (node.GetType() == NodeType::Exit) nodeColor = ImGuiColor::RED;
	else if (node.GetType() == NodeType::AnyState)	nodeColor = ImGuiColor::SPRING_GREEN;
	else if (node.GetType() == NodeType::State)	nodeColor = ImGuiColor::ORANGE;
	ed::PushStyleColor(ed::StyleColor_NodeBorder, nodeColor);

	auto nodeID = entt::hashed_string(name.c_str()).value();
	ed::BeginNode(nodeID);

	ImGui::Text(name.c_str());

	// �� 
	beginPin_AnimationStateNode(name, node.GetType());

	// Node ����
	if (ed::IsNodeSelected(nodeID) 
		&& ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		mEventManager->FireEvent<editor_event::SelectAnimationController>(
			{ mSelectController, name }
		);
	}

	ed::EndNode();

	ed::PopStyleColor(1);
}

void fq::game_engine::AnimatorWindow::beginPin_AnimationStateNode(const std::string& nodeName, fq::game_module::AnimationStateNode::Type type)
{
	if (type != game_module::AnimationStateNode::Type::AnyState
		&& type != game_module::AnimationStateNode::Type::Entry)
	{
		ed::BeginPin(getInputPinID(nodeName), ed::PinKind::Input);
		ImGui::Text("Enter");
		ed::EndPin();
	}
	else ImGui::Text("     ");

	ImGui::SameLine();

	if (type != game_module::AnimationStateNode::Type::Exit)
	{
		ed::BeginPin(getOutputPinID(nodeName), ed::PinKind::Output);
		ImGui::Text("Exit");
		ed::EndPin();
	}
}

fq::game_engine::AnimatorWindow::PinID fq::game_engine::AnimatorWindow::getInputPinID(const std::string& nodeName)
{
	auto inputPinName = nodeName + "__InputPin";
	auto pinID = entt::hashed_string(inputPinName.c_str()).value();

	mMatchPinID[pinID] = nodeName;

	return pinID;
}

fq::game_engine::AnimatorWindow::PinID fq::game_engine::AnimatorWindow::getOutputPinID(const std::string& nodeName)
{
	auto outputPinName = nodeName + "__OutputPin";
	auto pinID = entt::hashed_string(outputPinName.c_str()).value();

	mMatchPinID[pinID] = nodeName;

	return pinID;
}

void fq::game_engine::AnimatorWindow::beginCreate()
{
	if (ed::BeginCreate())
	{
		ed::PinId inputID, outputID;
		if (ed::QueryNewLink(&inputID, &outputID))
		{
			if (inputID && outputID)
			{
				if (ed::AcceptNewItem())
				{
					// Transition �߰�
					size_t exit = inputID.Get();
					size_t enter = outputID.Get();

					auto& exitState = mMatchPinID.find(exit)->second;
					auto& enterState = mMatchPinID.find(enter)->second;

					mSelectController->AddTransition(exitState, enterState);
				}
			}
		}
	}
	ed::EndCreate(); // Wrap up deletion action

}

void fq::game_engine::AnimatorWindow::beginDelete()
{
	if (ed::BeginDelete())
	{
		ed::LinkId deleteLinkID;
		while (ed::QueryDeletedLink(&deleteLinkID))
		{
			if (ed::AcceptDeletedItem())
			{
				// Transition ���� 
				spdlog::trace("delete");
				const auto& linkPair = mMatchLinkID.find(deleteLinkID.Get())->second;
				mSelectController->DeleteTransition(linkPair.first, linkPair.second);
			}
		}

	}
	ed::EndDelete();
}

void fq::game_engine::AnimatorWindow::beginLink_AnimationTransition(const fq::game_module::AnimationTransition& transition)
{
	auto exit = transition.GetExitState(); // exit
	auto exitID = getOutputPinID(exit);
	auto enter = transition.GetEnterState(); // enter 
	auto enterID = getInputPinID(enter);

	auto linkID = entt::hashed_string((exit + enter).c_str()).value();

	if (mMatchLinkID.find(linkID) == mMatchLinkID.end())
		mMatchLinkID.insert({ linkID, {exit, enter } });

	ed::Link(linkID, exitID, enterID);
}
