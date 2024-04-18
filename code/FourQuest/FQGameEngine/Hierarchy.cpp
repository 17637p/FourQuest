#include "Hierarchy.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "GameProcess.h"
#include "EditorProcess.h"
#include "CommandSystem.h"
#include "Command.h"
#include "EditorEvent.h"

fq::game_engine::Hierarchy::Hierarchy()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mScene(nullptr)
	, mInputManager(nullptr)
	, mEventManager(nullptr)
	, mSelectObject(nullptr)
	, mSelectObjectHandle{}
{}

fq::game_engine::Hierarchy::~Hierarchy()
{}


void fq::game_engine::Hierarchy::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mSelectObject = nullptr;

	mScene = mGameProcess->mSceneManager->GetCurrentScene();
	mEventManager = mGameProcess->mEventManager.get();
	mInputManager = mEditorProcess->mInputManager.get();

	// 이벤트 핸들 등록
	mSelectObjectHandle = mEventManager->RegisterHandle<editor_event::SelectObject>
		([this](editor_event::SelectObject event) {
		mSelectObject = event.object;
			});
}

void fq::game_engine::Hierarchy::Render()
{
	if (ImGui::Begin("Hierarchy"))
	{
		beginSearchBar();

		if (ImGui::BeginChild("HierarchyChild"))
		{
			beginPopupContextWindow_HierarchyChild();
			if (mSearchString.empty())
			{
				beginHierarchy();
			}
			else
			{
				beginHierarchyOfSearch();
			}
		}
		ImGui::EndChild();
		dragDropWindow();
	}
	ImGui::End();
}

void fq::game_engine::Hierarchy::beginPopupContextWindow_HierarchyChild()
{
	if (ImGui::BeginPopupContextWindow())
	{
		// GameObject를 생성합니다
		if (ImGui::MenuItem("CreateEmpty"))
		{
			mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene
				, std::make_shared<fq::game_module::GameObject>());
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::Hierarchy::beginSearchBar()
{
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Search", &mSearchString);
	ImGui::Separator();
}

void fq::game_engine::Hierarchy::beginHierarchy()
{
	// GameObject를 순회하면서 Bar를 생성합니다
	for (auto& object : mScene->GetObjectView(true))
	{
		// Root들만 정보를 표시합니다
		if (object.GetParent() != nullptr)
		{
			continue;
		}

		beginGameObjectBar(object);
	}
}


void fq::game_engine::Hierarchy::beginHierarchyOfSearch()
{
	// GameObject를 순회하면서 Bar를 생성합니다
	for (auto& object : mScene->GetObjectView(true))
	{
		std::string objectName = object.GetName();

		if (objectName.find(mSearchString) == std::string::npos)
		{
			continue;
		}

		beginGameObjectNameBar(object);
	}
}


void fq::game_engine::Hierarchy::beginGameObjectNameBar(fq::game_module::GameObject& object)
{
	std::string id = "##" + std::to_string(object.GetID());
	std::string objectName = object.GetName();

	ImGui::SetNextItemWidth(150.f);

	if (ImGui::InputText(id.c_str(), &objectName)
		&& mInputManager->IsKeyState(Key::Enter, KeyState::Tap))
	{
		std::shared_ptr<fq::game_module::GameObject> sObject
			= object.shared_from_this();

		// 이름이 수정된 경우 Set 명령어 전달합니다
		// 수명관리를 위해서 shared_ptr로 람다에 전달합니다
		mEditorProcess->mCommandSystem->Push<SetValueCommand<std::string>>(
			SetValueCommand<std::string>{ [sObject](std::string name)
			{
				sObject->SetName(name);
			}, sObject->GetName(), objectName }
		);
	}
}

void fq::game_engine::Hierarchy::beginGameObjectBar(fq::game_module::GameObject& object)
{
	auto* objectT = object.GetComponent<fq::game_module::Transform>();
	float cursorPosX = 0.f;

	//  위치 설정 
	if (!objectT->IsLeaf() || objectT->HasParent())
	{
		cursorPosX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + cursorPosX);
	}

	if (!objectT->HasParent() && objectT->IsLeaf())
	{
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + ImGui::GetCursorPos().x);
	}

	// 선택버튼 관련 처리
	begineGameObectSelectButton(object);
	beginPopupContextItem_GameObject(object);

	// 드래그앤드랍 처리 
	dragDropGameObjectBar(object);

	// 오브젝트 이름
	beginGameObjectNameBar(object);

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
				beginGameObjectBar(*child);
			}

			ImGui::TreePop();
		}
	}
}

void fq::game_engine::Hierarchy::dragDropGameObjectBar(fq::game_module::GameObject& object)
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
		const ImGuiPayload* PathPayLoad = ImGui::AcceptDragDropPayload("GameObject");

		if (PathPayLoad)
		{
			fq::game_module::GameObject* dropObject
				= static_cast<fq::game_module::GameObject*>(PathPayLoad->Data);

			auto* parentT = object.GetComponent<fq::game_module::Transform>();
			auto* childT = dropObject->GetComponent<fq::game_module::Transform>();

			if (childT->IsDescendant(parentT))
			{
				ImGui::EndDragDropTarget();
				return;
			}

			auto removeChild = [parentT, childT]()
				{
					parentT->RemoveChild(childT);
				};

			auto addChild = [parentT, childT]()
				{
					parentT->AddChild(childT);
				};

			if (childT->GetParentTransform() == parentT)
			{
				mEditorProcess->mCommandSystem->Push<BindFunctionCommand>
					(removeChild, addChild);
			}
			else
			{
				mEditorProcess->mCommandSystem->Push<BindFunctionCommand>
					(addChild, removeChild);
			}
		}
		ImGui::EndDragDropTarget();
	}

}

void fq::game_engine::Hierarchy::begineGameObectSelectButton(fq::game_module::GameObject& object)
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
		mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
				mGameProcess->mEventManager.get(), object.shared_from_this(), mSelectObject
			});
	}

	if (isSelected)
	{
		ImGui::PopStyleColor();
	}

	ImGui::SameLine();
}

void fq::game_engine::Hierarchy::beginPopupContextItem_GameObject(fq::game_module::GameObject& object)
{
	std::string ContextItemName = object.GetName() + "ContextItem";

	if (ImGui::BeginPopupContextItem(ContextItemName.c_str()))
	{
		// GameObject를 생성합니다
		if (ImGui::MenuItem("Destroy"))
		{
			mEditorProcess->mCommandSystem->Push<DestroyObjectCommand>(mScene
				, object.shared_from_this());
		}

		// GameObject를 생성합니다
		if (ImGui::MenuItem("CreateEmpty"))
		{
			mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene
				, std::make_shared<fq::game_module::GameObject>());
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::Hierarchy::dragDropWindow()
{
	// Drop 처리
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* path
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			// 일단은 Prefab은 json형식입니다
			if (path->extension() != ".prefab")
			{
				return;
			}

			auto prefab = mGameProcess->mObjectManager->LoadPrefab(*path);

			assert(!prefab.empty());

			// 부모를 등록하면 자식계층도 같이 등록합니다
			mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene
				, prefab[0]);
		}

		const ImGuiPayload* objectPayLoad = ImGui::AcceptDragDropPayload("GameObject");

		if (objectPayLoad)
		{
			fq::game_module::GameObject* dropObject
				= static_cast<fq::game_module::GameObject*>(objectPayLoad->Data);

			auto parent =  dropObject->GetParent();

			if (parent != nullptr)
			{
				dropObject->GetComponent<fq::game_module::Transform>()->SetParent(nullptr);
			}
		}

		ImGui::EndDragDropTarget();
	}


}

void fq::game_engine::Hierarchy::Finalize()
{
	mEventManager->RemoveHandle(mSelectObjectHandle);
}

