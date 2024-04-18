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

	// �̺�Ʈ �ڵ� ���
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
		// GameObject�� �����մϴ�
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
	// GameObject�� ��ȸ�ϸ鼭 Bar�� �����մϴ�
	for (auto& object : mScene->GetObjectView(true))
	{
		// Root�鸸 ������ ǥ���մϴ�
		if (object.GetParent() != nullptr)
		{
			continue;
		}

		beginGameObjectBar(object);
	}
}


void fq::game_engine::Hierarchy::beginHierarchyOfSearch()
{
	// GameObject�� ��ȸ�ϸ鼭 Bar�� �����մϴ�
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

		// �̸��� ������ ��� Set ��ɾ� �����մϴ�
		// ��������� ���ؼ� shared_ptr�� ���ٿ� �����մϴ�
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

	//  ��ġ ���� 
	if (!objectT->IsLeaf() || objectT->HasParent())
	{
		cursorPosX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + cursorPosX);
	}

	if (!objectT->HasParent() && objectT->IsLeaf())
	{
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + ImGui::GetCursorPos().x);
	}

	// ���ù�ư ���� ó��
	begineGameObectSelectButton(object);
	beginPopupContextItem_GameObject(object);

	// �巡�׾ص�� ó�� 
	dragDropGameObjectBar(object);

	// ������Ʈ �̸�
	beginGameObjectNameBar(object);

	// Hierarchy ���
	auto children = object.GetChildren();

	// �ڽİ����� ����� �����մϴ�
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

	// Drag ó��
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::Text(objectName.c_str());
		ImGui::SetDragDropPayload("GameObject", &object
			, sizeof(fq::game_module::GameObject));
		ImGui::EndDragDropSource();
	}

	// Drop ó��
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
	// ���ù�ư
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
		// GameObject�� �����մϴ�
		if (ImGui::MenuItem("Destroy"))
		{
			mEditorProcess->mCommandSystem->Push<DestroyObjectCommand>(mScene
				, object.shared_from_this());
		}

		// GameObject�� �����մϴ�
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
	// Drop ó��
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* path
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			// �ϴ��� Prefab�� json�����Դϴ�
			if (path->extension() != ".prefab")
			{
				return;
			}

			auto prefab = mGameProcess->mObjectManager->LoadPrefab(*path);

			assert(!prefab.empty());

			// �θ� ����ϸ� �ڽİ����� ���� ����մϴ�
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

