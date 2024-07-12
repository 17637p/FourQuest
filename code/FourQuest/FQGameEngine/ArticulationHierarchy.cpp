#include "ArticulationHierarchy.h"

#include "imgui_stdlib.h"

#include "EditorProcess.h"
#include "GameProcess.h"
#include "CameraSystem.h"

#include "../FQCommon/FQPath.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/ArticulationData.h"
#include "../FQGameModule/LinkData.h"
#include "../FQGraphics/IFQGraphics.h"

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
		drawArticulationDebug(mArticulationData->GetRootLinkData().lock());

		if (ImGui::Begin("Articulation Hierarchy", &mbIsOpen))
		{
			ImGui::BeginChild("Articulation Hierarchy");

			beginHierarchyButton();
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

		beginPopupContextWindow_HierarchyChild();
		beginArticulationSaveButton();
		beginLinkDataBar(mArticulationData->GetRootLinkData().lock());
	}

	void ArticulationHierarchy::beginArticulationSaveButton()
	{
		static std::string articulationName = "Articulation File Name";

		ImGui::InputText("###", &articulationName);

		ImGui::SameLine();

		if (ImGui::Button("Save"))
		{
			std::string articulationPath = articulationName + ".articulation";

			auto path = fq::path::GetResourcePath() / "Articulation" / articulationPath;
			mArticulationLoader.Save(mArticulationData, path.c_str());
		}
	}

	void ArticulationHierarchy::beginLinkDataBar(std::shared_ptr<fq::game_module::LinkData> link)
	{
		float cursorPosX = 0.f;

		cursorPosX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + cursorPosX);

		// 선택버튼 관력 처리
		beginLinkSelectButton(link);
		beginPopupContextItem_Link(link);

		// 드래그앤 드랍 처리
		dragDropLinkBar(link);

		// 링크 이름
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
			// 이름이 수정된 경우 Set 명령어 전달합니다
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
		// 선택 버튼
		std::string buttonName = "##buttonName" + std::to_string(link->GetID());

		bool isSelected = mSelectLinkData != nullptr 
			&& mSelectLinkData->GetID() == link->GetID();

		if (isSelected)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 0.f, 1.f));
		}

		// 여기에 이제 링크가 선택 되었다는 이벤트를 추가하면 됩니다.
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
		if (object.GetComponent<fq::game_module::Light>() != nullptr
			|| object.GetComponent<fq::game_module::SkinnedMeshRenderer>() != nullptr)
			return;

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
			// Link를 생성합니다
			if (ImGui::MenuItem("Destroy"))
			{
				link->SetIsDead(true);
			}

			// Link를 생성합니다
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

		mEditorProcess->mGamePlayWindow->DrawGizumo(linkMatrix);

		linkMatrix = linkMatrix * mSelectLinkData->GetParentTransform().Invert();
		mSelectLinkData->SetLocalTransform(linkMatrix);
	}

	void ArticulationHierarchy::drawArticulationDebug(std::shared_ptr<fq::game_module::LinkData> linkData)
	{
		float jointAxisScale = 1.f;

		drawLinkDebug(linkData, jointAxisScale);
		drawJointAxisDebug(linkData, jointAxisScale);

		for (auto& childLinkData : linkData->GetChildrenLinkData())
		{
			drawArticulationDebug(childLinkData.second);
		}
	}

	void ArticulationHierarchy::drawLinkDebug(std::shared_ptr<fq::game_module::LinkData> linkData, float& jointAxisScale)
	{
		fq::game_module::EShapeType shapeType = linkData->GetShapeType();
		DirectX::SimpleMath::Color color = DirectX::SimpleMath::Color(1.f, 1.f, 0.f, 1.f);

		switch (shapeType)
		{
		case fq::game_module::EShapeType::BOX:
		{
			fq::graphics::debug::OBBInfo info;

			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion rotation;
			DirectX::SimpleMath::Vector3 position;

			DirectX::SimpleMath::Matrix worldTransform = linkData->GetWorldTransform();
			worldTransform.Decompose(scale, rotation, position);

			info.Color = color;
			info.OBB.Center = position;
			info.OBB.Orientation = rotation;
			info.OBB.Extents = linkData->GetBoxExtent();

			jointAxisScale = std::min(info.OBB.Extents.x, std::min(info.OBB.Extents.y, info.OBB.Extents.z));

			mGameProcess->mGraphics->DrawOBB(info);
		}
		break;
		case fq::game_module::EShapeType::SPHERE:
		{
			fq::graphics::debug::SphereInfo info;

			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion rotation;
			DirectX::SimpleMath::Vector3 position;

			DirectX::SimpleMath::Matrix worldTransform = linkData->GetWorldTransform();
			worldTransform.Decompose(scale, rotation, position);

			info.Color = color;
			info.Sphere.Center = position;
			info.Sphere.Radius = linkData->GetSphereRadius();

			jointAxisScale = info.Sphere.Radius;

			mGameProcess->mGraphics->DrawSphere(info);
		}
		break;
		case fq::game_module::EShapeType::CAPSULE:
		{
			fq::graphics::debug::SphereInfo sphereinfo1;
			fq::graphics::debug::SphereInfo sphereinfo2;
			fq::graphics::debug::RayInfo rayInfo1;
			fq::graphics::debug::RayInfo rayInfo2;
			fq::graphics::debug::RayInfo rayInfo3;
			fq::graphics::debug::RayInfo rayInfo4;

			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion rotation;
			DirectX::SimpleMath::Vector3 position;

			DirectX::SimpleMath::Matrix worldTransform = linkData->GetWorldTransform();
			worldTransform.Decompose(scale, rotation, position);

			sphereinfo1.Color = color;
			sphereinfo2.Color = color;
			rayInfo1.Color = color;
			rayInfo2.Color = color;
			rayInfo3.Color = color;
			rayInfo4.Color = color;

			sphereinfo1.Sphere.Center = position + worldTransform.Right() * linkData->GetCapsuleHalfHeight();
			sphereinfo2.Sphere.Center = position - worldTransform.Right() * linkData->GetCapsuleHalfHeight();
			sphereinfo1.Sphere.Radius = linkData->GetCapsuleRadius();
			sphereinfo2.Sphere.Radius = linkData->GetCapsuleRadius();
			rayInfo1.Normalize = false;
			rayInfo2.Normalize = false;
			rayInfo3.Normalize = false;
			rayInfo4.Normalize = false;
			rayInfo1.Origin = position + worldTransform.Forward() * linkData->GetCapsuleRadius() - worldTransform.Right() * linkData->GetCapsuleHalfHeight();
			rayInfo2.Origin = position - worldTransform.Forward() * linkData->GetCapsuleRadius() - worldTransform.Right() * linkData->GetCapsuleHalfHeight();
			rayInfo3.Origin = position + worldTransform.Up() * linkData->GetCapsuleRadius() - worldTransform.Right() * linkData->GetCapsuleHalfHeight();
			rayInfo4.Origin = position - worldTransform.Up() * linkData->GetCapsuleRadius() - worldTransform.Right() * linkData->GetCapsuleHalfHeight();
			rayInfo1.Direction = worldTransform.Right() * linkData->GetCapsuleHalfHeight() * 2;
			rayInfo2.Direction = worldTransform.Right() * linkData->GetCapsuleHalfHeight() * 2;
			rayInfo3.Direction = worldTransform.Right() * linkData->GetCapsuleHalfHeight() * 2;
			rayInfo4.Direction = worldTransform.Right() * linkData->GetCapsuleHalfHeight() * 2;

			jointAxisScale = sphereinfo1.Sphere.Radius;

			mGameProcess->mGraphics->DrawSphere(sphereinfo1);
			mGameProcess->mGraphics->DrawSphere(sphereinfo2);
			mGameProcess->mGraphics->DrawRay(rayInfo1);
			mGameProcess->mGraphics->DrawRay(rayInfo2);
			mGameProcess->mGraphics->DrawRay(rayInfo3);
			mGameProcess->mGraphics->DrawRay(rayInfo4);
		}
		break;
		default:
			break;
		}
	}

	void ArticulationHierarchy::drawJointAxisDebug(std::shared_ptr<fq::game_module::LinkData> linkData, const float& jointAxisScale)
	{
		DirectX::SimpleMath::Matrix jointTransform = linkData->GetJointLocalTransform() * linkData->GetWorldTransform();
		DirectX::SimpleMath::Vector3 jointPosition;
		DirectX::SimpleMath::Quaternion jointRotation;
		DirectX::SimpleMath::Vector3 jointScale;

		jointTransform.Decompose(jointScale, jointRotation, jointPosition);

		// 조인트 회전 축 렌더링 디버그
		if (linkData->GetSwing1AxisMotion() == fq::physics::EArticulationMotion::LIMITED)
		{
			fq::graphics::debug::RingInfoEx swing1Info;

			float swing1LimitLow = linkData->GetSwing1LimitLow() * (DirectX::XM_PI / 180.0);
			DirectX::SimpleMath::Matrix limitRotation = DirectX::SimpleMath::Matrix::CreateRotationY(-swing1LimitLow);
			swing1Info.Origin = jointPosition;
			swing1Info.MajorAxis = limitRotation.Right() / 3.f * jointAxisScale;
			swing1Info.MinorAxis = -limitRotation.Forward() / 3.f * jointAxisScale;
			swing1Info.Color = DirectX::SimpleMath::Color(1.f, 0.f, 0.f, 1.f);
			swing1Info.ArcInRadian = (linkData->GetSwing1LimitHigh() - linkData->GetSwing1LimitLow()) / 180.f * 3.14f;

			mGameProcess->mGraphics->DrawRingEx(swing1Info);
		}
		if (linkData->GetSwing2AxisMotion() == fq::physics::EArticulationMotion::LIMITED)
		{
			fq::graphics::debug::RingInfoEx swing2Info;

			float swing2LimitLow = linkData->GetSwing2LimitHigh() * (DirectX::XM_PI / 180.0);
			DirectX::SimpleMath::Matrix limitRotation =
				DirectX::SimpleMath::Matrix::CreateRotationZ(90.f * DirectX::XM_PI / 180.0)
				* DirectX::SimpleMath::Matrix::CreateRotationY(90.f * DirectX::XM_PI / 180.0)
				* DirectX::SimpleMath::Matrix::CreateRotationZ(swing2LimitLow);
			swing2Info.Origin = jointPosition;
			swing2Info.MajorAxis = limitRotation.Right() / 3.f * jointAxisScale;
			swing2Info.MinorAxis = -limitRotation.Forward() / 3.f * jointAxisScale;
			swing2Info.Color = DirectX::SimpleMath::Color(1.f, 0.f, 0.f, 1.f);
			swing2Info.ArcInRadian = (linkData->GetSwing2LimitHigh() - linkData->GetSwing2LimitLow()) / 180.f * 3.14f;

			mGameProcess->mGraphics->DrawRingEx(swing2Info);
		}
		if (linkData->GetTwistAxisMotion() == fq::physics::EArticulationMotion::LIMITED)
		{
			fq::graphics::debug::RingInfoEx twistInfo;

			float twistLimitLow = linkData->GetTwistLimitLow() * (DirectX::XM_PI / 180.0);
			DirectX::SimpleMath::Matrix limitRotation =
				DirectX::SimpleMath::Matrix::CreateRotationZ(90.f * DirectX::XM_PI / 180.0)
				* DirectX::SimpleMath::Matrix::CreateRotationX(-twistLimitLow);
			twistInfo.Origin = jointPosition;
			twistInfo.MajorAxis = limitRotation.Right() / 3.f * jointAxisScale;
			twistInfo.MinorAxis = limitRotation.Forward() / 3.f * jointAxisScale;
			twistInfo.Color = DirectX::SimpleMath::Color(1.f, 0.f, 0.f, 1.f);
			twistInfo.ArcInRadian = (linkData->GetTwistLimitHigh() - linkData->GetTwistLimitLow()) / 180.f * 3.14f;

			mGameProcess->mGraphics->DrawRingEx(twistInfo);
		}
	}

	void ArticulationHierarchy::beginHierarchyButton()
	{
		bool bisCheck = false;

		if (mbIsBoneHierarchy)
		{
			bisCheck = true;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 0.f, 0.5f));
		}
		if (ImGui::Button("BoneHierarchy"))
		{
			mbIsBoneHierarchy = true;
			mbIsArticulationHierarchy = false;
		}
		if (bisCheck)
		{
			bisCheck = false;
			ImGui::PopStyleColor();
		}
		ImGui::SameLine();

		if (mbIsArticulationHierarchy)
		{
			bisCheck = true;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.f, 0.f, 0.5f));
		}
		if (ImGui::Button("ArticulationHierarchy"))
		{
			mbIsBoneHierarchy = false;
			mbIsArticulationHierarchy = true;
		}
		if (bisCheck)
		{
			bisCheck = false;
			ImGui::PopStyleColor();
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

				// Articulation 로드
				if (path->extension() == ".articulation")
				{
					mArticulationData = mArticulationLoader.LoadArticulationData(*path, mLinkID);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}
