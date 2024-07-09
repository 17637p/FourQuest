#include "ArticulationInspector.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "EditorEvent.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "../FQGameModule/Articulation.h"
#include "../FQGameModule/ArticulationData.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::game_engine
{
	ArticulationInspector::ArticulationInspector()
		: mSelectLink(nullptr)
	{
	}

	ArticulationInspector::~ArticulationInspector()
	{
	}

	void ArticulationInspector::Initialize(GameProcess* game, EditorProcess* editor)
	{
		mGameProcess = game;
		mEditorProcess = editor;

		mArticulationData = std::make_shared<fq::game_module::ArticulationData>();

		// 이벤트 핸들 등록
		mSelectObjectHandler = mGameProcess->mEventManager->RegisterHandle<fq::event::SelectLinkData>
			([this](fq::event::SelectLinkData event) {
			mArticulationData = event.articulationData;
			mSelectLink = event.linkData;
				});
	}

	void ArticulationInspector::Render()
	{
		if (!mbIsOpen) return;

		if (ImGui::Begin("Articulation Inspector", &mbIsOpen))
		{
			beginLinkData();
		}
		ImGui::End();
	}

	bool beginJointCombo(fq::physics::EArticulationMotion& motion, const std::string& comboName)
	{
		const char* items[] = { "Lock", "Limit", "Free" };
		int itemCurrentIdx = (int)motion;
		if (ImGui::BeginCombo(comboName.c_str(), items[itemCurrentIdx]))
		{
			// 콤보 박스와 각 항목을 추가합니다.
			for (int i = 0; i < (int)fq::physics::EArticulationMotion::END; i++)
			{
				const bool isSelected = (itemCurrentIdx == i);

				// 항목이 선택되었을 때 인덱스를 업데이트합니다.
				if (ImGui::Selectable(items[i], isSelected))
				{
					motion = (fq::physics::EArticulationMotion)i;
				}

				// 현재 선택된 항목을 강조합니다.
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		if (itemCurrentIdx == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void beginShapeCombo(int& itemCurrentIdx, const std::string& comboName)
	{
		const char* items[] = { "Box", "Sphere", "Capsule" };
		if (ImGui::BeginCombo(comboName.c_str(), items[itemCurrentIdx]))
		{
			// 콤보 박스와 각 항목을 추가합니다.
			for (int i = 0; i < 3; i++)
			{
				const bool isSelected = (itemCurrentIdx == i);

				// 항목이 선택되었을 때 인덱스를 업데이트합니다.
				if (ImGui::Selectable(items[i], isSelected))
				{
					itemCurrentIdx = i;
				}

				// 현재 선택된 항목을 강조합니다.
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}

	void game_engine::ArticulationInspector::beginLinkData()
	{
		// Articulation
		if (ImGui::TreeNode("Articulation"))
		{
			float articulationDensity = mArticulationData->GetDensity();
			float articulationDynamicFriction = mArticulationData->GetDynamicFriction();
			float articulationRestitution = mArticulationData->GetRestitution();
			float articulationStaticFriction = mArticulationData->GetStaticFriction();

			ImGui::InputFloat("ArticulationDensity", &articulationDensity);
			ImGui::InputFloat("ArticulationDynamicFriction", &articulationDynamicFriction);
			ImGui::InputFloat("ArticulationRestitution", &articulationRestitution);
			ImGui::InputFloat("ArticulationStaticFriction", &articulationStaticFriction);

			mArticulationData->SetDensity(articulationDensity);
			mArticulationData->SetDynamicFriction(articulationDynamicFriction);
			mArticulationData->SetRestitution(articulationRestitution);
			mArticulationData->SetStaticFriction(articulationStaticFriction);

			ImGui::TreePop();
		}

		ImGui::Separator();

		// Link
		if (ImGui::TreeNode("Link"))
		{
			if (mSelectLink)
			{
				std::string parentBoneName = mSelectLink->GetParentBoneName();
				std::string boneName = mSelectLink->GetBoneName();
				float density = mSelectLink->GetDensity();
				DirectX::SimpleMath::Matrix localTransform = mSelectLink->GetLocalTransform();

				DirectX::SimpleMath::Vector3 extent = mSelectLink->GetBoxExtent();
				float sphereRadius = mSelectLink->GetSphereRadius();
				float capsuleHalfHeight = mSelectLink->GetCapsuleHalfHeight();
				float capsuleRadius = mSelectLink->GetCapsuleRadius();

				DirectX::SimpleMath::Vector3 linkPosition;
				DirectX::SimpleMath::Quaternion linkRotation;
				DirectX::SimpleMath::Vector3 linkScale;
				localTransform.Decompose(linkScale, linkRotation, linkPosition);

				ImGui::InputFloat3("LinkScale", (float*)&linkScale);
				ImGui::InputFloat4("LinkRotation", (float*)&linkRotation);
				ImGui::InputFloat3("LinkPosition", (float*)&linkPosition);
				ImGui::InputFloat("Density", &density);

				ImGui::Separator();

				static int shapeNumber = 0;
				beginShapeCombo(shapeNumber, "Link Shape");

				switch (shapeNumber)
				{
				case 0:
				{
					ImGui::InputFloat3("Extent", (float*)&extent);
				}
				break;
				case 1:
				{
					ImGui::InputFloat("Radius", &sphereRadius);
				}
				break;
				case 2:
				{
					ImGui::InputFloat("Half Height", &capsuleHalfHeight);
					ImGui::InputFloat("Radius", &capsuleRadius);
				}
				break;
				default:
					break;
				}

				localTransform = DirectX::SimpleMath::Matrix::CreateScale(linkScale)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(linkRotation)
					* DirectX::SimpleMath::Matrix::CreateTranslation(linkPosition);

				mSelectLink->SetParentBoneName(parentBoneName);
				mSelectLink->SetBoneName(boneName);
				mSelectLink->SetDensity(density);
				mSelectLink->SetLocalTransform(localTransform);
				mSelectLink->SetBoxExtent(extent);
				mSelectLink->SetSphereRadius(sphereRadius);
				mSelectLink->SetCapsuleHalfHeight(capsuleHalfHeight);
				mSelectLink->SetCapsuleRadius(capsuleRadius);
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

		// Joint
		if (ImGui::TreeNode("Joint"))
		{
			if (mSelectLink)
			{
				DirectX::SimpleMath::Matrix jointLocalTransform = mSelectLink->GetJointLocalTransform();
				float jointDamping = mSelectLink->GetJointDamping();
				float jointMaxForce = mSelectLink->GetJointMaxForce();
				float jointStiffness = mSelectLink->GetJointStiffness();

				DirectX::SimpleMath::Vector3 jointPosition;
				DirectX::SimpleMath::Quaternion jointRotation;
				DirectX::SimpleMath::Vector3 jointScale;
				jointLocalTransform.Decompose(jointScale, jointRotation, jointPosition);

				ImGui::InputFloat3("JointScale", (float*)&jointScale);
				ImGui::InputFloat4("JointRotation", (float*)&jointRotation);
				ImGui::InputFloat3("JointPosition", (float*)&jointPosition);
				ImGui::InputFloat("JointDamping", &jointDamping);
				ImGui::InputFloat("JointMaxForce", &jointMaxForce);
				ImGui::InputFloat("JointStiffness", &jointStiffness);

				jointLocalTransform = DirectX::SimpleMath::Matrix::CreateScale(jointScale)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(jointRotation)
					* DirectX::SimpleMath::Matrix::CreateTranslation(jointPosition);

				mSelectLink->SetJointDamping(jointDamping);
				mSelectLink->SetJointLocalTransform(jointLocalTransform);
				mSelectLink->SetJointMaxForce(jointMaxForce);
				mSelectLink->SetJointStiffness(jointStiffness);

				ImGui::Separator();

				// JointAxis
				if (ImGui::TreeNode("JointAxis"))
				{
					fq::physics::EArticulationMotion swing1Motion = mSelectLink->GetSwing1AxisMotion();
					float swing1LimitHigh = mSelectLink->GetSwing1LimitHigh();
					float swing1LimitLow = mSelectLink->GetSwing1LimitLow();
					fq::physics::EArticulationMotion swing2Motion = mSelectLink->GetSwing2AxisMotion();
					float swing2LimitHigh = mSelectLink->GetSwing2LimitHigh();
					float swing2LimitLow = mSelectLink->GetSwing2LimitLow();
					fq::physics::EArticulationMotion twistMotion = mSelectLink->GetTwistAxisMotion();
					float twistLimitHigh = mSelectLink->GetTwistLimitHigh();
					float twistLimitLow = mSelectLink->GetTwistLimitLow();

					if (beginJointCombo(swing1Motion, "Swing1 Motion"))
					{
						ImGui::InputFloat("Swing1 Limit High", &swing1LimitHigh);
						ImGui::InputFloat("Swing1 Limit Low", &swing1LimitLow);
					}
					ImGui::Separator();
					if (beginJointCombo(swing2Motion, "Swing2 Motion"))
					{
						ImGui::InputFloat("Swing2 Limit High", &swing2LimitHigh);
						ImGui::InputFloat("Swing2 Limit Low", &swing2LimitLow);
					}
					ImGui::Separator();
					if (beginJointCombo(twistMotion, "Twist Motion"))
					{
						ImGui::InputFloat("Twist Limit High", &twistLimitHigh);
						ImGui::InputFloat("Twist Limit Low", &twistLimitLow);
					}

					mSelectLink->SetSwing1AxisMotion(swing1Motion);
					mSelectLink->SetSwing1LimitHigh(swing1LimitHigh);
					mSelectLink->SetSwing1LimitLow(swing1LimitLow);
					mSelectLink->SetSwing2AxisMotion(swing2Motion);
					mSelectLink->SetSwing2LimitHigh(swing2LimitHigh);
					mSelectLink->SetSwing2LimitLow(swing2LimitLow);
					mSelectLink->SetTwistAxisMotion(twistMotion);
					mSelectLink->SetTwistLimitHigh(twistLimitHigh);
					mSelectLink->SetTwistLimitLow(twistLimitLow);

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}
}
