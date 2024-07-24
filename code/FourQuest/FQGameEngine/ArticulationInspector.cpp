#include "ArticulationInspector.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "EditorEvent.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "../FQGameModule/LinkData.h"

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
			beginArticulationData();
			ImGui::Separator();
			beginLinkData();
			ImGui::Separator();
			beginJointData();
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

	void beginShapeCombo(const std::string& comboName, fq::game_module::EShapeType& shapeType)
	{
		const char* items[] = { "Box", "Sphere", "Capsule", "None"};
		if (ImGui::BeginCombo(comboName.c_str(), items[(int)shapeType]))
		{
			// 콤보 박스와 각 항목을 추가합니다.
			for (int i = 0; i < 4; i++)
			{
				const bool isSelected = (shapeType == (fq::game_module::EShapeType)i);

				// 항목이 선택되었을 때 인덱스를 업데이트합니다.
				if (ImGui::Selectable(items[i], isSelected))
				{
					shapeType = (fq::game_module::EShapeType)i;
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

	void ArticulationInspector::beginArticulationData()
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
	}

	void game_engine::ArticulationInspector::beginLinkData()
	{
		// Link
		if (ImGui::TreeNode("Link"))
		{
			if (mSelectLink)
			{
				std::string parentBoneName = mSelectLink->GetParentBoneName();
				std::string boneName = mSelectLink->GetBoneName();
				float density = mSelectLink->GetDensity();
				DirectX::SimpleMath::Matrix localTransform = mSelectLink->GetLocalTransform();

				fq::game_module::EShapeType shapeType = mSelectLink->GetShapeType();
				DirectX::SimpleMath::Vector3 extent = mSelectLink->GetBoxExtent();
				float sphereRadius = mSelectLink->GetSphereRadius();
				float capsuleHalfHeight = mSelectLink->GetCapsuleHalfHeight();
				float capsuleRadius = mSelectLink->GetCapsuleRadius();

				DirectX::SimpleMath::Vector3 linkPosition;
				DirectX::SimpleMath::Quaternion linkRotation;
				DirectX::SimpleMath::Vector3 linkScale;
				localTransform.Decompose(linkScale, linkRotation, linkPosition);

				DirectX::SimpleMath::Vector3 euler = linkRotation.ToEuler();

				float f[3]{ DirectX::XMConvertToDegrees(euler.x)
					,DirectX::XMConvertToDegrees(euler.y)
					,DirectX::XMConvertToDegrees(euler.z) };

				ImGui::InputFloat3("LinkScale", (float*)&linkScale);
				ImGui::InputFloat3("LinkRotation", f);
				ImGui::InputFloat3("LinkPosition", (float*)&linkPosition);
				ImGui::InputFloat("Density", &density);

				ImGui::Separator();

				beginShapeCombo("Link Shape", shapeType);

				switch (shapeType)
				{
				case fq::game_module::EShapeType::BOX:
				{
					ImGui::InputFloat3("Extent", (float*)&extent);
				}
				break;
				case fq::game_module::EShapeType::SPHERE:
				{
					ImGui::InputFloat("Radius", &sphereRadius);
				}
				break;
				case fq::game_module::EShapeType::CAPSULE:
				{
					ImGui::InputFloat("Half Height", &capsuleHalfHeight);
					ImGui::InputFloat("Radius", &capsuleRadius);
				}
				break;
				default:
					break;
				}

				euler.x = DirectX::XMConvertToRadians(f[0]);
				euler.y = DirectX::XMConvertToRadians(f[1]);
				euler.z = DirectX::XMConvertToRadians(f[2]);
				linkRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(euler);

				localTransform = DirectX::SimpleMath::Matrix::CreateScale(linkScale)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(linkRotation)
					* DirectX::SimpleMath::Matrix::CreateTranslation(linkPosition);

				mSelectLink->SetParentBoneName(parentBoneName);
				mSelectLink->SetBoneName(boneName);
				mSelectLink->SetDensity(density);
				mSelectLink->SetLocalTransform(localTransform);
				mSelectLink->SetShapeType(shapeType);
				mSelectLink->SetBoxExtent(extent);
				mSelectLink->SetSphereRadius(sphereRadius);
				mSelectLink->SetCapsuleHalfHeight(capsuleHalfHeight);
				mSelectLink->SetCapsuleRadius(capsuleRadius);
			}

			ImGui::TreePop();
		}
	}

	void ArticulationInspector::beginJointData()
	{
		// Joint
		if (ImGui::TreeNode("Joint"))
		{
			if (mSelectLink)
			{
				DirectX::SimpleMath::Matrix jointLocalTransform = mSelectLink->GetJointLocalTransform();
				float jointDamping = mSelectLink->GetJointDamping();
				float jointMaxForce = mSelectLink->GetJointMaxForce();
				float jointStiffness = mSelectLink->GetJointStiffness();

				//DirectX::SimpleMath::Vector3 jointPosition;
				//DirectX::SimpleMath::Quaternion jointRotation;
				//DirectX::SimpleMath::Vector3 jointScale;
				//jointLocalTransform.Decompose(jointScale, jointRotation, jointPosition);

				//DirectX::SimpleMath::Vector3 euler = jointRotation.ToEuler();

				//float f[3]{ DirectX::XMConvertToDegrees(euler.x)
				//	,DirectX::XMConvertToDegrees(euler.y)
				//	,DirectX::XMConvertToDegrees(euler.z) };

				//ImGui::InputFloat3("JointScale", (float*)&jointScale);
				//ImGui::InputFloat3("JointRotation", f);
				//ImGui::InputFloat3("JointPosition", (float*)&jointPosition);

				if (ImGui::Button("Bone Transform Connect"))
				{
					auto scene = mGameProcess->mSceneManager->GetCurrentScene();

					// GameObject를 순회하면서 Bar를 생성합니다
					for (auto& object : scene->GetObjectView(true))
					{
						if (object.GetComponent<fq::game_module::Animator>() != nullptr)
						{
							auto animatorMesh = object.GetComponent<fq::game_module::Animator>();

							if (animatorMesh->GetHasNodeHierarchyInstance() == false)
								return;

							auto objectTransform = object.GetComponent<fq::game_module::Transform>();
							auto objectScale = objectTransform->GetWorldScale();

							auto& nodeHierarchy = animatorMesh->GetNodeHierarchyInstance();

							DirectX::SimpleMath::Matrix boneRootTransform = nodeHierarchy.GetRootTransform(mSelectLink->GetBoneName()) 
								* DirectX::SimpleMath::Matrix::CreateScale(objectScale);

							DirectX::SimpleMath::Matrix linkWorldTransform = mSelectLink->GetWorldTransform();
							linkWorldTransform._11 = 1.f;
							linkWorldTransform._22 = 1.f;
							linkWorldTransform._33 = 1.f;

							DirectX::SimpleMath::Vector3 position;
							DirectX::SimpleMath::Quaternion rotation;
							DirectX::SimpleMath::Vector3 scale;

							boneRootTransform = boneRootTransform * linkWorldTransform.Invert();
							boneRootTransform.Decompose(scale, rotation, position);

							mSelectLink->SetJointLocalTransform(position, rotation);
						}
					}
				}
				//else
				//{
				//	euler.x = DirectX::XMConvertToRadians(f[0]);
				//	euler.y = DirectX::XMConvertToRadians(f[1]);
				//	euler.z = DirectX::XMConvertToRadians(f[2]);
				//	jointRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(euler);

				//	mSelectLink->SetJointLocalTransform(jointPosition, jointRotation);
				//}

				ImGui::InputFloat("JointDamping", &jointDamping);
				ImGui::InputFloat("JointMaxForce", &jointMaxForce);
				ImGui::InputFloat("JointStiffness", &jointStiffness);

				mSelectLink->SetJointDamping(jointDamping);
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