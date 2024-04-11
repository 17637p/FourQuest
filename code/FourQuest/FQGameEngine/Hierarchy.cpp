#include "Hierarchy.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "CommandSystem.h"
#include "Command.h"

fq::game_engine::Hierarchy::Hierarchy()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mScene(nullptr)
	, mInputManager(nullptr)
	, mSelectObject(nullptr)
{}

fq::game_engine::Hierarchy::~Hierarchy()
{}


void fq::game_engine::Hierarchy::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mSelectObject = nullptr;

	mScene = mGameProcess->mSceneManager->GetCurrentScene();
	mInputManager = mEditorProcess->mInputManager.get();
}

void fq::game_engine::Hierarchy::Render()
{
	if (ImGui::Begin("Hierarchy"))
	{
		BeginPopupContextWindow();
		BeginSearchBar();

		if (mSearchString.empty())
		{
			BeginHierarchy();
		}
		else
		{
			BeginHierarchyOfSearch();
		}

		ImGui::End();
	}
}

void fq::game_engine::Hierarchy::BeginPopupContextWindow()
{
	if (ImGui::BeginPopupContextWindow())
	{
		// GameObject를 생성합니다
		if (ImGui::MenuItem("CreateEmpty"))
		{
			mEditorProcess->mCommandSystem->Push<CreateObject>(mScene
				, std::make_shared<fq::game_module::GameObject>());
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::Hierarchy::BeginSearchBar()
{
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Search", &mSearchString);
	ImGui::Separator();
}

void fq::game_engine::Hierarchy::BeginHierarchy()
{
	// GameObject를 순회하면서 Bar를 생성합니다
	for (auto& object : mScene->GetObjectView(true))
	{
		// Root들만 정보를 표시합니다
		if (object.GetParent() != nullptr)
		{
			continue;
		}

		BeginGameObjectBar(object);
	}
}


void fq::game_engine::Hierarchy::BeginHierarchyOfSearch()
{
	// GameObject를 순회하면서 Bar를 생성합니다
	for (auto& object : mScene->GetObjectView(true))
	{
		std::string objectName = object.GetName();

		if (objectName.find(mSearchString) == std::string::npos)
		{
			continue;
		}

		BeginGameObjectNameBar(object);
	}
}


void fq::game_engine::Hierarchy::BeginGameObjectNameBar(fq::game_module::GameObject& object)
{
	std::string id = "##" + std::to_string(object.GetID());
	std::string objectName = object.GetName();

	ImGui::SetNextItemWidth(150.f);

	if (ImGui::InputText(id.c_str(), &objectName)
		&& mInputManager->IsKeyState(Key::Enter, KeyState::Tap))
	{
		std::shared_ptr<fq::game_module::GameObject> sObject
			= mScene->GetObjectByID(object.GetID());

		// 이름이 수정된 경우 Set 명령어 전달합니다
		// 수명관리를 위해서 shared_ptr로 람다에 전달합니다
		mEditorProcess->mCommandSystem->Push<SetValue<std::string>>(
			SetValue<std::string>{ [sObject](std::string name)
			{
				sObject->SetName(name);
			}, sObject->GetName(), objectName }
		);
	}
}

void fq::game_engine::Hierarchy::BeginGameObjectBar(fq::game_module::GameObject& object)
{
	auto* objectT = object.GetComponent<fq::game_module::Transform>();
	float cursorPosX = 0.f;
	
	if (!objectT->IsLeaf() || objectT->HasParent())
	{
		cursorPosX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + cursorPosX);
	}

	// 선택버튼 관련 처리
	BegineGameObectSelectButton(object);
	
	// 드래그앤드랍 처리 
	DragDropGameObject(object);

	// 오브젝트 이름
	BeginGameObjectNameBar(object);
	
	// Hierarchy 토글
	auto children = object.GetChildren();

	// 자식계층의 토글을 생성합니다
	if (!children.empty())
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosX);

		std::string treeNodeName = "##Tree" + std::to_string(object.GetID());
		if (ImGui::TreeNode(treeNodeName.c_str()))
		{
			for (auto& child : children)
			{
				BeginGameObjectBar(*child);
			}

			ImGui::TreePop();
		}
	}
}

void fq::game_engine::Hierarchy::DragDropGameObject(fq::game_module::GameObject& object)
{
	std::string objectName = object.GetName();

	// Drag 처리
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::Text(objectName.c_str());
		ImGui::SetDragDropPayload("GameObject", &object
			, sizeof(fq::game_module::GameObject));
		ImGui::EndDragDropSource();
	}

	// Drop 처리
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("GameObject");

		if (payLoad)
		{
			fq::game_module::GameObject* dropObject
				= static_cast<fq::game_module::GameObject*>(payLoad->Data);

			auto* parentT = object.GetComponent<fq::game_module::Transform>();
			auto* childT = dropObject->GetComponent<fq::game_module::Transform>();

			if (childT->IsDescendant(parentT))
			{
				return;
			}

			if (childT->GetParentTransform() == parentT)
			{
				parentT->RemoveChild(childT);
			}
			else
			{
				parentT->AddChild(childT);
			}
		}
	}

}

void fq::game_engine::Hierarchy::BegineGameObectSelectButton(fq::game_module::GameObject& object)
{
	// 선택버튼
	std::string buttonName = " ##buttonName" + std::to_string(object.GetID());

	bool isSelected = mSelectObject != nullptr
		&& mSelectObject->GetID() == object.GetID();

	if (isSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.0f, 0.f, 1.f));
	}

	if (ImGui::Button(buttonName.c_str()))
	{
		mSelectObject = mScene->GetObjectByID(object.GetID());
	}

	if (isSelected)
	{
		ImGui::PopStyleColor();
	}

	ImGui::SameLine();
}

