#include "ArticulationHierarchy.h"

#include "imgui_stdlib.h"

#include "EditorProcess.h"
#include "GameProcess.h"
#include "CameraSystem.h"

#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/ArticulationData.h"
#include "../FQGameModule/LinkData.h"

namespace fq::game_engine
{
	ArticulationHierarchy::ArticulationHierarchy()
		: mGameProcess(nullptr)
		, mEditorProcess(nullptr)
		, mScene(nullptr)
		, mEventManager(nullptr)
		, mObject(nullptr)
		, mSkinnedMeshRenderer(nullptr)
		, mBones()
		, mArticulationData(nullptr)
		, mSelectLinkData(nullptr)
		, mSelectLinkHandle()
		, mbIsOpen(false)
		, mbIsBoneHierarchy(true)
		, mbIsArticulationHierarchy(false)
		, mLinkID(1)
		, mOperation(ImGuizmo::TRANSLATE)
	{
	}

	ArticulationHierarchy::~ArticulationHierarchy()
	{
	}

	void ArticulationHierarchy::Initialize(GameProcess* game, EditorProcess* editor)
	{
		mGameProcess = game;
		mEditorProcess = editor;

		mScene = mGameProcess->mSceneManager->GetCurrentScene();
		mEventManager = mGameProcess->mEventManager.get();
		mInputManager = mEditorProcess->mInputManager.get();

		mArticulationData = std::make_shared<fq::game_module::ArticulationData>();

		mSelectLinkHandle = mEventManager->RegisterHandle<fq::event::SelectLinkData>
			([this](fq::event::SelectLinkData event)
				{
					mSelectLinkData = event.linkData;
				});
	}

	void ArticulationHierarchy::Finalize()
	{
	}

	void ArticulationHierarchy::Render()
	{
		if (!mbIsOpen) return;

		mArticulationData->Update();

		if (ImGui::Begin("Articulation Hierarchy", &mbIsOpen))
		{
			ImGui::BeginChild("Articulation Hierarchy");
			
			if (ImGui::Button("BoneHierarchy"))
			{
				mbIsBoneHierarchy = true;
				mbIsArticulationHierarchy = false;
			} 
			ImGui::SameLine();
			if (ImGui::Button("ArticulationHierarchy"))
			{
				mbIsBoneHierarchy = false;
				mbIsArticulationHierarchy = true;
			}

			beginBoneHierarchy();
			beginArticulationHierarchy();
			beginGizmo();

			ImGui::EndChild();
			dragDropGameWindow();
		}
		ImGui::End();
	}

	void ArticulationHierarchy::beginBoneHierarchy()
	{
		if (!mbIsBoneHierarchy)
			return;

		// GameObject�� ��ȸ�ϸ鼭 Bar�� �����մϴ�
		for (auto& object : mScene->GetObjectView(true))
		{
			// Root�鸸 ������ ǥ���մϴ�
			if (object.GetParent() != nullptr || object.IsDestroyed())
			{
				continue;
			}

			beginGameObjectBar(object);
		}
	}

	void ArticulationHierarchy::beginArticulationHierarchy()
	{
		if (!mbIsArticulationHierarchy)
			return;

		beginPopupContextWindow_HierarchyChild();
		beginLinkDataBar(mArticulationData->GetRootLinkData().lock());
	}

	void ArticulationHierarchy::beginLinkDataBar(std::shared_ptr<fq::game_module::LinkData> link)
	{
		float cursorPosX = 0.f;

		cursorPosX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + cursorPosX);

		// ���ù�ư ���� ó��
		beginLinkSelectButton(link);
		beginPopupContextItem_Link(link);

		// �巡�׾� ��� ó��
		dragDropLinkBar(link);

		// ��ũ �̸�
		beginLinkDataNameBar(link);

		auto children = link->GetChildrenLinkData();

		if (!children.empty())
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorPosX);

			std::string treeNodeName = "##Tree" + std::to_string(link->GetID());

			if (ImGui::TreeNode(treeNodeName.c_str()))
			{
				for (auto& childLink : link->GetChildrenLinkData())
				{
					beginLinkDataBar(childLink.second);
				}

				ImGui::TreePop();
			}
		}
	}

	void ArticulationHierarchy::beginLinkDataNameBar(std::shared_ptr<fq::game_module::LinkData> link)
	{
		std::string id = "##" + std::to_string(link->GetID());
		std::string linkName = link->GetBoneName();

		ImGui::SetNextItemWidth(150.f);

		if (ImGui::InputText(id.c_str(), &linkName)
			&& mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
		{
			// �̸��� ������ ��� Set ��ɾ� �����մϴ�
			link->SetBoneName(linkName);

			for (auto& childLink : link->GetChildrenLinkData())
			{
				childLink.second->SetParentBoneName(linkName);
			}

			linkName = link->GetBoneName();
		}
	}

	void ArticulationHierarchy::beginLinkSelectButton(std::shared_ptr<fq::game_module::LinkData> link)
	{
		// ���� ��ư
		std::string buttonName = "##buttonName" + std::to_string(link->GetID());

		bool isSelected = mSelectLinkData != nullptr 
			&& mSelectLinkData->GetID() == link->GetID();

		if (isSelected)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 0.f, 1.f));
		}

		// ���⿡ ���� ��ũ�� ���� �Ǿ��ٴ� �̺�Ʈ�� �߰��ϸ� �˴ϴ�.
		if (ImGui::Button(buttonName.c_str()))
		{
			mSelectLinkData = link;

			mEventManager->FireEvent<fq::event::SelectLinkData>(
				{ mArticulationData, mSelectLinkData }
			);
		}

		if (isSelected)
		{
			ImGui::PopStyleColor();
		}

		ImGui::SameLine();
	}

	void ArticulationHierarchy::beginGameObjectBar(fq::game_module::GameObject& object)
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

			// ������ ������Ʈ�� �ڽ� ������ ��쿡�� TreeNode�� ��Ĩ�ϴ�
			ImGui::SetNextItemOpen(true);

			if (ImGui::TreeNode(treeNodeName.c_str()))
			{
				for (auto& child : children)
				{
					if (!child->IsDestroyed())
						beginGameObjectBar(*child);
				}

				ImGui::TreePop();
			}
		}
	}

	void ArticulationHierarchy::beginGameObjectNameBar(fq::game_module::GameObject& object)
	{
		std::string id = "##" + std::to_string(object.GetID());
		std::string objectName = object.GetName();

		ImGui::SetNextItemWidth(150.f);

		if (ImGui::InputText(id.c_str(), &objectName)
			&& mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
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


	void ArticulationHierarchy::beginPopupContextWindow_HierarchyChild()
	{
		if (ImGui::BeginPopupContextWindow())
		{
			// GameObject�� �����մϴ�
			if (ImGui::MenuItem("CreateEmpty"))
			{
				std::shared_ptr<fq::game_module::LinkData> rootLink = mArticulationData->GetRootLinkData().lock();
				std::shared_ptr<fq::game_module::LinkData> newLink = std::make_shared<fq::game_module::LinkData>();
				newLink->SetBoneName("newLink" + std::to_string(mLinkID));
				newLink->SetParentBoneName(rootLink->GetBoneName());
				newLink->SetID(mLinkID++);
				rootLink->AddChildLinkData(newLink->GetBoneName(), newLink);
			}

			ImGui::EndPopup();
		}
	}

	void ArticulationHierarchy::beginPopupContextItem_Link(std::shared_ptr<fq::game_module::LinkData> link)
	{
		std::string ContextItemName = link->GetID() + "ContextItem";

		if (ImGui::BeginPopupContextItem(ContextItemName.c_str()))
		{
			// Link�� �����մϴ�
			if (ImGui::MenuItem("Destroy"))
			{
				link->SetIsDead(true);
			}

			// Link�� �����մϴ�
			if (ImGui::MenuItem("CreateEmpty"))
			{
				std::shared_ptr<fq::game_module::LinkData> newLink = std::make_shared<fq::game_module::LinkData>();

				newLink->SetBoneName("newLink" + std::to_string(mLinkID));
				newLink->SetParentBoneName(link->GetBoneName());
				newLink->SetID(mLinkID++);
				link->AddChildLinkData(newLink->GetBoneName(), newLink);
			}

			ImGui::EndPopup();
		}
	}

	void addLinkData(std::shared_ptr<fq::game_module::LinkData> tourLink, std::shared_ptr<fq::game_module::LinkData> link, std::string addLinkName)
	{
		for (auto& childLink : tourLink->GetChildrenLinkData())
		{
			addLinkData(childLink.second, link, addLinkName);
		}

		if (tourLink == link)
		{
			return;
		}

		std::shared_ptr<fq::game_module::LinkData> moveLink = tourLink->GetChildLinkData(addLinkName);

		if (moveLink != nullptr)
		{
			link->AddChildLinkData(addLinkName, tourLink->GetChildLinkData(addLinkName));
			return;
		}
	}

	void removeLinkData(std::shared_ptr<fq::game_module::LinkData> tourLink, std::shared_ptr<fq::game_module::LinkData> link, std::string removeLinkName)
	{
		for (auto& childLink : tourLink->GetChildrenLinkData())
		{
			removeLinkData(childLink.second, link, removeLinkName);
		}

		if (tourLink == link)
		{
			return;
		}

		tourLink->RemoveChildLinkData(removeLinkName);
	}

	void ArticulationHierarchy::dragDropLinkBar(std::shared_ptr<fq::game_module::LinkData> link)
	{
		std::string linkName = link->GetBoneName();

		// Drag ó��
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::Text(linkName.c_str());
			ImGui::SetDragDropPayload("LinkData", link.get()
				, sizeof(fq::game_module::LinkData));
			ImGui::EndDragDropSource();
		}

		// Drop ó��
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* PathPayLoad = ImGui::AcceptDragDropPayload("LinkData");

			if (PathPayLoad)
			{
				fq::game_module::LinkData* dropLink = static_cast<fq::game_module::LinkData*>(PathPayLoad->Data);

				for (auto& childLink : link->GetChildrenLinkData())
				{
					if (childLink.first == dropLink->GetBoneName())
					{
						ImGui::EndDragDropTarget();
						return;
					}
				}

				addLinkData(mArticulationData->GetRootLinkData().lock(), link, dropLink->GetBoneName());
				removeLinkData(mArticulationData->GetRootLinkData().lock(), link, dropLink->GetBoneName());
			}

			ImGui::EndDragDropTarget();
		}
	}

	void ArticulationHierarchy::beginGizmo()
	{
		if (mSelectLinkData == nullptr)
		{
			return;
		}

		DirectX::SimpleMath::Matrix linkMatrix = mSelectLinkData->GetWorldTransform();
		DirectX::SimpleMath::Matrix beforeMatrix = mSelectLinkData->GetWorldTransform();

		mEditorProcess->mGamePlayWindow->DrawGizumo(linkMatrix);

		mSelectLinkData->SetLocalTransform(linkMatrix);
	}

	void ArticulationHierarchy::dragDropGameWindow()
	{
		// Drop ó��
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* PathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (PathPayLoad)
			{
				auto path = static_cast<std::filesystem::path*>(PathPayLoad->Data);

				// �� ����
				if (path->extension() == ".model")
				{
					//// ������ ���� ���� ����
					//mGameProcess->mEventManager 
					//	->FireEvent<fq::event::RequestChangeScene>({ "Articulation_Editer", true });

					// ���� �ε�
					mEditorProcess->mModelSystem->BuildModel(*path);

					for (auto& object : mScene->GetObjectView())
					{
						if (object.HasComponent<fq::game_module::SkinnedMeshRenderer>())
						{
							mObject = mScene->GetObjectByID(object.GetID());
							mSkinnedMeshRenderer = object.GetComponent<fq::game_module::SkinnedMeshRenderer>();
							break;
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}
