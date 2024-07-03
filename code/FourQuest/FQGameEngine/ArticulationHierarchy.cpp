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

	void ArticulationHierarchy::beginArticulationHierarchy()
	{
		if (!mbIsArticulationHierarchy)
			return;


	}

	void ArticulationHierarchy::beginPopupContextWindow_HierarchyChild()
	{
		if (ImGui::BeginPopupContextWindow())
		{
			// GameObject를 생성합니다
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
		//	// GameObject를 생성합니다
		//	if (ImGui::MenuItem("Destroy"))
		//	{
		//		mEditorProcess->mCommandSystem->Push<DestroyObjectCommand>(mScene
		//			, object.shared_from_this());
		//	}

		//	// GameObject를 생성합니다
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
