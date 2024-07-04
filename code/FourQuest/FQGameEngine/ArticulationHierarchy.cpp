#include "ArticulationHierarchy.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "EditorProcess.h"
#include "GameProcess.h"

#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/ArticulationData.h"

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
		, mSelectObjectHandle()
		, mbIsOpen(false)
		, mbIsBoneHierarchy(true)
		, mbIsArticulationHierarchy(false)
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
	}

	void ArticulationHierarchy::Finalize()
	{
	}

	void ArticulationHierarchy::Render()
	{
		if (!mbIsOpen) return;

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

	void ArticulationHierarchy::beginArticulationHierarchy()
	{
		if (!mbIsArticulationHierarchy)
			return;


	}

	void ArticulationHierarchy::beginPopupContextWindow_HierarchyChild()
	{
		if (ImGui::BeginPopupContextWindow())
		{
			// GameObject�� �����մϴ�
			if (ImGui::MenuItem("CreateEmpty"))
			{
				
			}

			ImGui::EndPopup();
		}
	}

	void ArticulationHierarchy::beginPopupContextItem_Link()
	{
		//if (ImGui::BeginPopupContextItem(ContextItemName.c_str()))
		//{
		//	// GameObject�� �����մϴ�
		//	if (ImGui::MenuItem("Destroy"))
		//	{
		//		mEditorProcess->mCommandSystem->Push<DestroyObjectCommand>(mScene
		//			, object.shared_from_this());
		//	}

		//	// GameObject�� �����մϴ�
		//	if (ImGui::MenuItem("CreateEmpty"))
		//	{
		//		mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene
		//			, std::make_shared<fq::game_module::GameObject>());
		//	}

		//	ImGui::EndPopup();
		//}
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
