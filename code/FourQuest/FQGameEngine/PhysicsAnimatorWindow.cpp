#include "PhysicsAnimatorWindow.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "GameProcess.h"
#include "EditorProcess.h"
#include "PhysicsSystem.h"

#include "../FQphysics/IFQPhysics.h"
#include "../FQCommon/FQCommonLoader.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::game_engine
{
	PhysicsAnimatorWindow::PhysicsAnimatorWindow()
		: mGameProcess(nullptr)
		, mbIsOpen(false)
		, mRegisteredObject(nullptr)
		, mEventManager(nullptr)
		, mStartSceneHandle()
		, mKeyFrameSize()
		, mDurationTime()
		, mPlayAnimationClipNumber()
	{
	}

	PhysicsAnimatorWindow::~PhysicsAnimatorWindow()
	{

	}

	void PhysicsAnimatorWindow::Initialize(GameProcess* game)
	{
		mGameProcess = game;

		mScene = mGameProcess->mSceneManager->GetCurrentScene();
		mEventManager = mGameProcess->mEventManager.get();

		// �̺�Ʈ �ڵ� ���
		mStartSceneHandle = mEventManager->RegisterHandle<fq::event::StartScene>
			([this]() {
			StartScene();
				});
	}

	void PhysicsAnimatorWindow::Render()
	{
		if (!mbIsOpen) return;

		if (ImGui::Begin("PhysicsAnimator", &mbIsOpen))
		{
			ImGui::BeginChild("PhysicsAnimatorTool");
			beginAnimatiorEditor();
			ImGui::EndChild();
			dragDropGameObject();
		}
		ImGui::End();
	}

	void PhysicsAnimatorWindow::UpdateAnimation(float dt)
	{
		// �ִϸ��̼� ������ ����
		if (mbIsStartScene && mbIsOpen)
		{
			mDurationTime += dt;
			if (mDurationTime >= 1.f / 30.f)
			{
				for (auto& objectName : mExtractObjectNames)
				{
					auto object = mScene->GetObjectByName(objectName);
					DirectX::SimpleMath::Matrix localTransform = object->GetComponent<fq::game_module::Transform>()->GetLocalMatrix();

					mAnimationClip.find(objectName)->second.push_back(localTransform);
				}

				if (mCurrentKeyFrame >= mKeyFrameSize)
				{
					mCurrentKeyFrame = 0;
					mbIsStartScene = false;
					mAnimationClipContainer.push_back(mAnimationClip);
					mAnimationClip.clear();
				}

				mDurationTime -= 1.f / 30.f;
				mCurrentKeyFrame++;
			}
		}
		// �ִϸ��̼� ����
		else if (mbIsPlay && !mbIsStartScene && mbIsOpen)
		{
			mDurationTime += dt;
			if (mDurationTime >= (1 / 30.f))
			{
				if (mCurrentKeyFrame >= mAnimationClipContainer[mPlayAnimationClipNumber].find(mExtractObjectNames[0])->second.size())
				{
					mbIsPlay = false;
					mCurrentKeyFrame = 0;
				}
				for (int i = 0; i < mAnimationClipContainer[mPlayAnimationClipNumber].size(); i++)
				{
					auto& objectName = mExtractObjectNames[i];
					auto object = mScene->GetObjectByName(objectName);
					fq::game_module::Transform* trnasform = object->GetComponent<fq::game_module::Transform>();

					DirectX::SimpleMath::Matrix Transform = mAnimationClipContainer[mPlayAnimationClipNumber].find(objectName)->second[mCurrentKeyFrame];
					trnasform->SetLocalMatrix(Transform);
				}

				mDurationTime -= (1 / 30.f);
				mCurrentKeyFrame++;
			}
		}
	}

	void PhysicsAnimatorWindow::StartScene()
	{
		mbIsStartScene = true;
		mbIsPlay = false;

		resetAnimation();

		for (auto& objectName : mExtractObjectNames)
		{
			auto object = mScene->GetObjectByName(objectName);
			fq::game_module::Transform* objectTransform = object->GetComponent<fq::game_module::Transform>();

			std::vector<DirectX::SimpleMath::Matrix> localTrnasform;
			localTrnasform.push_back(objectTransform->GetLocalMatrix());
			mAnimationClip.insert(std::make_pair(objectName, localTrnasform));
		}
	}

	void PhysicsAnimatorWindow::beginAnimatiorEditor()
	{
		if (mRegisteredObject != nullptr)
		{
			// �ִϸ����� â
			beginText_ObjectName();
			beginInputInt_AnimationSize();
			beginInputInt_KeyFrameSize();
			beginButton_AnimationReset();

			for (int clipNumber = 0; clipNumber < mAnimationClipContainer.size(); clipNumber++)
			{
				beginTree_Animation(clipNumber);
			}
		}
	}

	void PhysicsAnimatorWindow::beginInputInt_KeyFrameSize()
	{
		// �����͸� ������ Ű ������ ������ �Է�
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputInt("KeyFrameSize (1Second = 30 FPS)", &mKeyFrameSize);
	}

	void PhysicsAnimatorWindow::beginInputInt_AnimationSize()
	{
		// �����͸� ������ �ִϸ��̼� ������ �Է�
		std::string barName = "Current Animation Size(" + std::to_string(mAnimationSize) + "/" + std::to_string(mCurrentAnimaitionNumber) + ")";
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputInt(barName.c_str(), &mAnimationSize);
	}

	void PhysicsAnimatorWindow::beginText_ObjectName()
	{
		ImGui::Text(mRegisteredObject->GetName().c_str());
	}

	void PhysicsAnimatorWindow::beginTree_Animation(const int& number)
	{
		std::string name = "Animation" + std::to_string(number) + "(Frame : " + std::to_string(mAnimationClipContainer[mPlayAnimationClipNumber].find(mExtractObjectNames[0])->second.size()) + ")";

		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::InputText("Name", &mAnimationNames[number]);

			if (!mbIsPlay)
				beginButton_AnimationPlay(number);
			else
				beginButton_AnimationStop(number);
			ImGui::SameLine(80.f);
			beginButton_AnimationSave(number);
			ImGui::TreePop();
		}
	}

	void PhysicsAnimatorWindow::beginButton_AnimationPlay(const int& number)
	{
		// ������ �ִϸ��̼� �÷��� ( �÷��� ��ư ������ ������Ʈ �Ǹ鼭 �ش� �޽� ������Ʈ�鿡�� setTransform�� ���� �ִϸ��̼� �÷��� )
		if (ImGui::Button("Play"))
		{
			mPlayAnimationClipNumber = number;
			mDurationTime = 0.f;
			mbIsPlay = true;
		}
	}

	void PhysicsAnimatorWindow::beginButton_AnimationStop(const int& number)
	{
		// ������ �ִϸ��̼� �÷��� ( �÷��� ��ư ������ ������Ʈ �Ǹ鼭 �ش� �޽� ������Ʈ�鿡�� setTransform�� ���� �ִϸ��̼� �÷��� )
		if (ImGui::Button("Stop"))
		{
			mPlayAnimationClipNumber = number;
			mDurationTime = 0.f;
			mbIsPlay = false;
		}
	}

	void PhysicsAnimatorWindow::beginButton_AnimationReset()
	{
		// ������ �ִϸ��̼� �÷��� ( �÷��� ��ư ������ ������Ʈ �Ǹ鼭 �ش� �޽� ������Ʈ�鿡�� setTransform�� ���� �ִϸ��̼� �÷��� )
		if (ImGui::Button("Reset"))
		{
			mDurationTime = 0.f;
			mbIsPlay = false;

			resetAnimation();
		}
	}

	void PhysicsAnimatorWindow::beginButton_AnimationSave(const int& number)
	{
		// ������ �ִϸ��̼� ���̺� ( ���̺� ��ư ������ �ش� �ִϸ��̼� �����͸� ���� )
		if (ImGui::Button("Save"))
		{
			mCurrentAnimaitionNumber++;
			if (mCurrentAnimaitionNumber >= mAnimationSize)
				mCurrentAnimaitionNumber = 0;

			mEventManager->FireEvent<fq::event::WriteAnimation>(
				{ mAnimationNames[number], mAnimationClipContainer[number], mRegisteredObject.get(), (unsigned int)mAnimationSize }
			);
		}
	}

	void PhysicsAnimatorWindow::dragDropGameObject()
	{
		// Drop ó��
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* PathPayLoad = ImGui::AcceptDragDropPayload("GameObject");
			if (PathPayLoad)
			{
				mRegisteredObject = nullptr;
				mExtractObjectNames.clear();
				mAnimationClipContainer.clear();
				mAnimationNames.clear();

				auto object = static_cast<fq::game_module::GameObject*>(PathPayLoad->Data);

				mRegisteredObject = object->shared_from_this();

				std::vector<fq::game_module::GameObject*> gameObjects = mRegisteredObject->GetChildren();
				mExtractObjectNames.reserve(gameObjects.size());
				//mExtractObjectNames.push_back(mRegisteredObject->GetName());

				// �ִϸ��̼� �̸�
				std::string animationName;
				animationName = "AnimationName";
				for (int i = 0; i < 100; i++)
					mAnimationNames.push_back(animationName);

				for (auto& object : gameObjects)
				{
					if (object->HasComponent<fq::game_module::SkinnedMeshRenderer>() || object->HasComponent<fq::game_module::StaticMeshRenderer>())
					{
						if (!object->HasComponent<fq::game_module::RigidBody>())
							break;

						mExtractObjectNames.push_back(object->GetName());
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	void PhysicsAnimatorWindow::resetAnimation()
	{
		mCurrentKeyFrame = 0;

		for (auto& objectName : mExtractObjectNames)
		{
			auto object = mScene->GetObjectByName(objectName);
			fq::game_module::Transform* objectTransform = object->GetComponent<fq::game_module::Transform>();

			if (!mAnimationClipContainer.empty())
			{
				DirectX::SimpleMath::Matrix localMatrix = mAnimationClipContainer[mPlayAnimationClipNumber].find(objectName)->second[0];
				objectTransform->SetLocalMatrix(localMatrix);
			}
		}
	}
}
