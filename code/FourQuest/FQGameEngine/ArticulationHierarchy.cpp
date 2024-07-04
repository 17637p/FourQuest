#include "ArticulationHierarchy.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "EditorProcess.h"
#include "GameProcess.h"

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
		, mSelectObjectHandle()
		, mbIsOpen(false)
		, mbIsBoneHierarchy(true)
		, mbIsArticulationHierarchy(false)
		, mLinkID(1)
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

			ImGui::EndChild();
			dragDropGameWindow();
		}
		ImGui::End();
	}

	void ArticulationHierarchy::beginBoneHierarchy()
	{
		if (!mbIsBoneHierarchy)
			return;

		// GameObject를 순회하면서 Bar를 생성합니다
		for (auto& object : mScene->GetObjectView(true))
		{
			// Root들만 정보를 표시합니다
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

		beginLinkDataBar(mArticulationData->GetRootLinkData().lock());
	}

	void ArticulationHierarchy::beginLinkDataBar(std::shared_ptr<fq::game_module::LinkData> link)
	{
		float cursorPosX = 0.f;

		cursorPosX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + cursorPosX);

		// 선택버튼 관력 처리
		beginLinkSelectButton(link);
		beginPopupContextItem_Link(link);



		// 링크 이름
		beginLinkDataNameBar(link);

		for (auto& childLink : link->GetChildrenLinkData())
		{
			beginLinkDataBar(childLink.second);
		}
	}

	void ArticulationHierarchy::beginLinkDataNameBar(std::shared_ptr<fq::game_module::LinkData> link)
	{
		std::string linkName = link->GetBoneName();

		ImGui::SetNextItemWidth(150.f);

		if (ImGui::InputText("##", &linkName)
			&& mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
		{
			// 이름이 수정된 경우 Set 명령어 전달합니다
			link->SetBoneName(linkName);

			for (auto& childLink : link->GetChildrenLinkData())
			{
				childLink.second->SetParentBoneName(linkName);
			}
		}
	}

	void ArticulationHierarchy::beginLinkSelectButton(std::shared_ptr<fq::game_module::LinkData> link)
	{
		// 선택 버튼
		std::string buttonName = "##buttonName" + link->GetID();

		bool isSelected = mSelectLinkData != nullptr 
			&& mSelectLinkData->GetID() == link->GetID();

		if (isSelected)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 0.f, 1.f));
		}

		// 여기에 이제 링크가 선택 되었다는 이벤트를 추가하면 됩니다.
		if (ImGui::Button(buttonName.c_str()))
		{

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

			// 선택한 오브젝트가 자식 계층인 경우에서 TreeNode를 펼칩니다
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


	void ArticulationHierarchy::beginPopupContextWindow_HierarchyChild()
	{
		if (ImGui::BeginPopupContextWindow())
		{
			// GameObject를 생성합니다
			if (ImGui::MenuItem("CreateEmpty"))
			{
				std::shared_ptr<fq::game_module::LinkData> rootLink = mArticulationData->GetRootLinkData().lock();
				std::shared_ptr<fq::game_module::LinkData> newLink = std::make_shared<fq::game_module::LinkData>();
				newLink->SetBoneName("newLink");
				newLink->SetParentBoneName(rootLink->GetBoneName());
				newLink->SetID(mLinkID++);
				rootLink->AddChildLinkData(newLink->GetBoneName(), newLink);
			}

			ImGui::EndPopup();
		}
	}

	void ArticulationHierarchy::beginPopupContextItem_Link(std::shared_ptr<fq::game_module::LinkData> link)
	{
		std::string ContextItemName = link->GetBoneName();

		if (ImGui::BeginPopupContextItem(ContextItemName.c_str()))
		{
			// GameObject를 생성합니다
			if (ImGui::MenuItem("Destroy"))
			{
				link->SetIsDead(true);
			}

			// GameObject를 생성합니다
			if (ImGui::MenuItem("CreateEmpty"))
			{
				std::shared_ptr<fq::game_module::LinkData> newLink = std::make_shared<fq::game_module::LinkData>();

				newLink->SetBoneName("newLink");
				newLink->SetParentBoneName(link->GetBoneName());
				link->AddChildLinkData(newLink->GetBoneName(), newLink);
				link->SetID(mLinkID++);
			}

			ImGui::EndPopup();
		}
	}

	void ArticulationHierarchy::dragDropLinkBar(std::shared_ptr<fq::game_module::LinkData> link)
	{
		std::string linkName = link->GetBoneName();

		// Drag 처리
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::Text(linkName.c_str());
			ImGui::SetDragDropPayload("LinkData", link.get()
				, sizeof(fq::game_module::LinkData));
			ImGui::EndDragDropSource();
		}

		// Drop 처리
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

				//for ()
				
			}
		}
	}

	void ArticulationHierarchy::dragDropGameWindow()
	{
		// Drop 처리
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* PathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (PathPayLoad)
			{
				auto path = static_cast<std::filesystem::path*>(PathPayLoad->Data);

				// 모델 생성
				if (path->extension() == ".model")
				{
					//// 에디터 전용 씬을 생성
					//mGameProcess->mEventManager
					//	->FireEvent<fq::event::RequestChangeScene>({ "Articulation_Editer", true });

					// 모델을 로드
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
