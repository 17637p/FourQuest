#include "AnimatorWindow.h"

#include <imgui-node-editor/imgui_node_editor.h>

#include "../FQCommon/FQPath.h"
#include "imgui_stdlib.h"
#include "EditorProcess.h"
#include "GameProcess.h"
#include "ImGuiColor.h"
#include "EditorEvent.h"

namespace ed = ax::NodeEditor;

fq::game_engine::AnimatorWindow::AnimatorWindow()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mbIsOpen(false)
	, mContext(nullptr)
	, mMatchPinID{}
	, mEventManager(nullptr)
	, mSelectController(nullptr)
	, mSelectControllerPath{}
	, mSelectObjectName{}
	, mOnLoadSceneHandler{}
	, mStartSceneHandler{}
	, mSelectObjectHandler{}
	, mSelectNodeID(0)
	, mbIsFocused(false)
{}

fq::game_engine::AnimatorWindow::~AnimatorWindow()
{}

void fq::game_engine::AnimatorWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mEventManager = mGameProcess->mEventManager.get();

	mOnLoadSceneHandler = mEventManager
		->RegisterHandle<fq::event::OnUnloadScene>(this, &AnimatorWindow::OnUnloadScene);

	mStartSceneHandler = mEventManager
		->RegisterHandle<fq::event::StartScene>(this, &AnimatorWindow::OnStartScene);

	mSelectObjectHandler = mEventManager
		->RegisterHandle<fq::editor_event::SelectObject>(this, &AnimatorWindow::SelectObject);
}

void fq::game_engine::AnimatorWindow::Render()
{
	if (!mbIsOpen) return;

	auto& io = ImGui::GetIO();

	if (ImGui::Begin("Animator", &mbIsOpen))
	{
		mbIsFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

		beginChild_ParameterWindow();
		ImGui::SameLine();
		beginChild_NodeEditor();
		dragDropWindow();
	}
	ImGui::End();
}

void fq::game_engine::AnimatorWindow::Finalize()
{
	destroyContext();
}

void fq::game_engine::AnimatorWindow::beginChild_ParameterWindow()
{
	if (ImGui::BeginChild("Parameter", ImVec2(150, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX))
	{
		if (!mSelectController)
		{
			ImGui::EndChild();
			return;
		}

		// Add Parametr
		beginCombo_AddParameter();

		// Parameters
		auto parameters = mSelectController->GetParameterPack();

		std::string originName;
		std::string changedName;

		auto eraseParameter = parameters.end();

		for (auto iter = parameters.begin(); iter != parameters.end(); ++iter)
		{
			const auto& id = iter->first;
			auto& parameter = iter->second;

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

			// 삭제 파라미터
			if (ImGui::BeginPopupContextItem(label.c_str()))
			{
				if (ImGui::MenuItem("Delete"))
					eraseParameter = iter;

				ImGui::EndPopup();
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

		// 파라미터 삭제처리
		if (parameters.end() != eraseParameter)
		{
			mSelectController->EraseParameter(eraseParameter->first);
		}

	}
	ImGui::EndChild();
}

void fq::game_engine::AnimatorWindow::beginChild_NodeEditor()
{
	if (ImGui::BeginChild("Node", ImVec2(0, 0)))
	{
		if (!mSelectController)
		{
			ImGui::EndChild();
			return;
		}

		ed::SetCurrentEditor(mContext);
		ImGui::Separator();
		ed::Begin("NodeEditor", ImVec2(0.0, 0.0f));

		ed::Style& style = ed::GetStyle();
		style.LinkStrength = 0.f;

		const auto& stateMap = mSelectController->GetStateMap();

		// Node, Pin
		for (const auto& [stateName, state] : stateMap)
		{
			beginNode_AnimationStateNode(stateName, state);
		}

		auto& transitions = mSelectController->GetTransitionMap();
		auto currentTransition = mSelectController->GetCurrentTransition();

		// Link
		for (auto iter = transitions.begin(); iter != transitions.end(); ++iter)
		{
			bool onFlow = false;

			if (iter == currentTransition)
			{
				onFlow = true;
			}

			beginLink_AnimationTransition(iter->second, onFlow);
		}

		beginCreate();
		beginDelete();
		ed::End();

		ed::SetCurrentEditor(nullptr);
		beginPopupContextWindow_NodeEditor();

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
	else if (node.GetType() == NodeType::AnyState)	nodeColor = ImGuiColor::SPRING_GREEN;
	else if (node.GetType() == NodeType::State)	nodeColor = ImGuiColor::ORANGE;
	ed::PushStyleColor(ed::StyleColor_NodeBorder, nodeColor);
	ed::PushStyleColor(ed::StyleColor_NodeBg, ImGuiColor::DARK_GRAY);

	auto nodeID = entt::hashed_string(name.c_str()).value();

	if (mMatchNodeID.find(nodeID) == mMatchNodeID.end())
		mMatchNodeID.insert({ nodeID, name });

	ed::BeginNode(nodeID);

	constexpr float nodeWidth = 150.f;

	// 노드 이름(중앙 정렬)
	float cursorPosX = ImGui::GetCursorPosX() + nodeWidth * 0.5f
		- ImGui::CalcTextSize(name.c_str()).x * 0.5f;
	ImGui::SetCursorPosX(cursorPosX);
	ImGui::Text(name.c_str());

	// 노드 기본사이즈 설정
	ImGui::Dummy(ImVec2(nodeWidth, 0.f));

	// 핀 
	beginPin_AnimationStateNode(name, node.GetType());

	// 애니메이션 진행상황 표시
	if (node.GetType() == NodeType::State
		&& mSelectController->GetCurrentStateName() == name
		&& !mSelectController->IsInTransition())
	{
		float duration = node.GetDuration();
		float timePos = mSelectController->GetTimePos();
		float ratio = timePos / duration;

		ImGui::ProgressBar(ratio, ImVec2(nodeWidth, 10.f), "");
	}
	else if (node.GetType() == NodeType::State
		&& mSelectController->GetNextStateName() == name
		&& mSelectController->IsInTransition())
	{
		const auto& state = mSelectController->GetStateMap().find(mSelectController->GetNextStateName());
		float duration = state->second.GetDuration();
		float blendPos = mSelectController->GetBlendTimePos();
		float ratio = blendPos / duration;

		ImGui::ProgressBar(ratio, ImVec2(nodeWidth, 10.f), "");
	}
	else
	{
		ImGui::Dummy(ImVec2(nodeWidth, 10.f));
	}

	// Node 선택
	if (ed::IsNodeSelected(nodeID) && mSelectNodeID != nodeID)
	{
		mSelectNodeID = nodeID;
		mEventManager->FireEvent<editor_event::SelectAnimationState>(
			{ mSelectController, name }
		);
	}

	ed::EndNode();

	ed::PopStyleColor(2);
}

void fq::game_engine::AnimatorWindow::beginPin_AnimationStateNode(const std::string& nodeName, fq::game_module::AnimationStateNode::Type type)
{
	constexpr float nodeWidth = 150.f;
	float cursorPosX = ImGui::GetCursorPosX() + nodeWidth / 2 - ImGui::CalcTextSize("I").x;
	ImGui::SetCursorPosX(cursorPosX);

	if (type != game_module::AnimationStateNode::Type::AnyState
		&& type != game_module::AnimationStateNode::Type::Entry)
	{
		ed::BeginPin(getInputPinID(nodeName), ed::PinKind::Input);
		ImGui::Text("O");
		ed::EndPin();

		if (type != game_module::AnimationStateNode::Type::Exit)
			ImGui::SameLine();
	}


	if (type != game_module::AnimationStateNode::Type::Exit)
	{
		ImGui::SetCursorPosX(cursorPosX);
		ed::BeginPin(getOutputPinID(nodeName), ed::PinKind::Output);
		ImGui::Text("O");
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
	if (ed::BeginCreate() && mbIsFocused)
	{
		ed::PinId inputID, outputID;
		if (ed::QueryNewLink(&inputID, &outputID))
		{
			if (inputID && outputID)
			{
				if (ed::AcceptNewItem())
				{
					// Transition 추가
					size_t exit = inputID.Get();
					size_t enter = outputID.Get();

					auto& exitState = mMatchPinID.find(exit)->second;
					auto& enterState = mMatchPinID.find(enter)->second;

					game_module::AnimationTransition transition{ exitState,enterState };
					mSelectController->AddTransition(transition);
				}
			}
		}
	}
	ed::EndCreate(); // Wrap up deletion action
}

void fq::game_engine::AnimatorWindow::beginDelete()
{
	if (ed::BeginDelete() && mbIsFocused)
	{
		ed::LinkId deleteLinkID;
		while (ed::QueryDeletedLink(&deleteLinkID))
		{
			if (ed::AcceptDeletedItem())
			{
				// Transition 삭제 
				const auto& linkPair = mMatchLinkID.find(deleteLinkID.Get())->second;
				mSelectController->DeleteTransition(linkPair.first, linkPair.second);
			}
		}

		ed::NodeId nodeID;
		while (ed::QueryDeletedNode(&nodeID))
		{
			if (ed::AcceptDeletedItem())
			{
				auto& stateName = mMatchNodeID.find(nodeID.Get())->second;
				mSelectController->DeleteState(stateName);
			}
		}
	}
	ed::EndDelete();
}

void fq::game_engine::AnimatorWindow::beginLink_AnimationTransition(const fq::game_module::AnimationTransition& transition, bool onFlow)
{
	auto exit = transition.GetExitState(); // exit
	auto exitID = getOutputPinID(exit);
	auto enter = transition.GetEnterState(); // enter 
	auto enterID = getInputPinID(enter);

	auto linkID = entt::hashed_string((exit + enter).c_str()).value();

	if (mMatchLinkID.find(linkID) == mMatchLinkID.end())
		mMatchLinkID.insert({ linkID, {exit, enter } });

	ed::Link(linkID, exitID, enterID, ImVec4{ 1,1,1,1 }, 1.f);

	if (onFlow)
	{
		ed::PushStyleVar(ax::NodeEditor::StyleVar_FlowDuration, 0.1f);
		ed::Flow(linkID);
		ed::PopStyleVar(1);
	}
}

void fq::game_engine::AnimatorWindow::dragDropWindow()
{
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* path
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			if (path->extension() == ".controller")
			{
				mSelectController = mLoader.Load(*path);
				mSelectControllerPath = *path;
				mSelectObjectName = {};
				createContext();
			}
		}

		const ImGuiPayload* objectPayLoad = ImGui::AcceptDragDropPayload("GameObject");
		if (objectPayLoad)
		{
			fq::game_module::GameObject* dropObject
				= static_cast<fq::game_module::GameObject*>(objectPayLoad->Data);

			auto animator = dropObject->GetComponent<fq::game_module::Animator>();

			if (animator)
			{
				auto controller = animator->GetSharedController();

				if (controller)
				{
					mSelectControllerPath = animator->GetControllerPath();
					mSelectController = controller;
					mSelectObjectName = dropObject->GetName();
					createContext();
				}
			}
		}
	}
}

void fq::game_engine::AnimatorWindow::createContext()
{
	destroyContext();

	auto relativePath = fq::path::GetRelativePath(mSelectControllerPath);
	auto hash = entt::hashed_string(relativePath.string().c_str()).value();

	mSettingFilePath = "resource/internal/animator/" + std::to_string(hash)
		+ ".json";

	// Context 생성
	ed::Config config;
	config.SettingsFile = mSettingFilePath.c_str();
	mContext = ed::CreateEditor(&config);
}

void fq::game_engine::AnimatorWindow::destroyContext()
{
	mMatchNodeID.clear();
	mMatchPinID.clear();
	mMatchLinkID.clear();

	if (mContext)
	{
		ed::DestroyEditor(mContext);
		mContext = nullptr;
	}
}

void fq::game_engine::AnimatorWindow::OnStartScene()
{
	// 게임을 시작하는 경우 애니메이터를 연결해줍니다 
	if (!mSelectObjectName.empty())
	{
		auto object = mGameProcess->mSceneManager->GetCurrentScene()->GetObjectByName(mSelectObjectName);

		if (object)
		{
			auto animator = object->GetComponent<fq::game_module::Animator>();

			if (animator)
			{
				mSelectController = animator->GetSharedController();
				mSelectControllerPath = animator->GetControllerPath();
			}
		}
	}
}

void fq::game_engine::AnimatorWindow::SaveAnimatorController()
{
	if (!mSelectController) return;

	mLoader.Save(*mSelectController, mSelectControllerPath);
}

void fq::game_engine::AnimatorWindow::OnUnloadScene()
{
	mSelectController = nullptr;
	mSelectControllerPath.clear();
}

void fq::game_engine::AnimatorWindow::SelectObject(fq::editor_event::SelectObject event)
{
	if (event.object == nullptr) return;

	if (event.object->HasComponent<game_module::Animator>())
	{
		auto animator = event.object->GetComponent<game_module::Animator>();
		mSelectObjectName = event.object->GetName(); 
		mSelectController = animator->GetSharedController();
		mSelectControllerPath = animator->GetControllerPath();
		createContext();
	}
}

