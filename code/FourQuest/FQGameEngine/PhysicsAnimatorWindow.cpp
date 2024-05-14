#include "PhysicsAnimatorWindow.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "GameProcess.h"
#include "EditorProcess.h"
#include "PhysicsSystem.h"

#include "../FQphysics/IFQPhysics.h"
#include "../FQCommon/FQCommonLoader.h"
#include "../FQGameModule/TimeManager.h"

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
		static int currentKeyFrame = 0;

		if (mbIsPlay)
		{
			if (mbIsStartScene)
			{
				for (int i = 0; i < mAnimationClipContainer[mPlayAnimationClipNumber].size(); i++)
				{
					fq::game_module::Transform* trnasform = mExtractObjects[i]->GetComponent<fq::game_module::Transform>();
					unsigned int objectID = mExtractObjects[i]->GetID();

					DirectX::SimpleMath::Matrix Transform = mAnimationClipContainer[mPlayAnimationClipNumber].find(objectID)->second[0];
					trnasform->SetLocalMatrix(Transform);
				}
			}
			else
			{
				mDurationTime += dt;
				if (mDurationTime >= (1 / 30.f))
				{
					mDurationTime -= (1 / 30.f);
					currentKeyFrame++;
				}

				if (currentKeyFrame > mAnimationClipContainer[mPlayAnimationClipNumber].size())
				{
					mbIsPlay = false;
					currentKeyFrame = 0;
				}

				for (int i = 0; i < mAnimationClipContainer[mPlayAnimationClipNumber].size(); i++)
				{
					fq::game_module::Transform* trnasform = mExtractObjects[i]->GetComponent<fq::game_module::Transform>();
					unsigned int objectID = mExtractObjects[i]->GetID();

					DirectX::SimpleMath::Matrix Transform = mAnimationClipContainer[mPlayAnimationClipNumber].find(objectID)->second[currentKeyFrame];
					trnasform->SetLocalMatrix(Transform);
				}
			}
		}

		if (mbIsStartScene)
		{
			mDurationTime += dt;

			for (auto& object : mExtractObjects)
			{
				DirectX::SimpleMath::Matrix localTransform = object->GetComponent<fq::game_module::Transform>()->GetLocalMatrix();

				mAnimationClip.find(object->GetID())->second.push_back(localTransform);
			}

			currentKeyFrame++;

			if (currentKeyFrame > mKeyFrameSize)
			{
				mbIsStartScene = false;
				mAnimationClipContainer.push_back(mAnimationClip);
				mAnimationClip.clear();
			}
		}
	}

	void PhysicsAnimatorWindow::StartScene()
	{
		mbIsStartScene = true;
		mDurationTime = 0.f;

		for (auto& object : mExtractObjects)
		{
			std::vector<DirectX::SimpleMath::Matrix> localTrnasform;
			mAnimationClip.insert(std::make_pair(object->GetID(), localTrnasform));
		}
	}

	void PhysicsAnimatorWindow::beginAnimatiorEditor()
	{
		if (mRegisteredObject != nullptr)
		{
			// �ִϸ����� â
			beginText_ObjectName();
			beginInputInt_KeyFrameSize();

			for (int clipNumber = 0; clipNumber < mAnimationClipContainer.size(); clipNumber++)
			{
				beginTree_Animation(clipNumber);
			}
		}
	}

	void PhysicsAnimatorWindow::beginInputInt_KeyFrameSize()
	{
		// �����͸� ������ Ű ������ ������ �Է�
		ImGui::InputInt("KeyFrameSize (1Second = 30 FPS)", &mKeyFrameSize);
	}

	void PhysicsAnimatorWindow::beginText_ObjectName()
	{
		ImGui::Text(mRegisteredObject->GetName().c_str());
	}

	void PhysicsAnimatorWindow::beginTree_Animation(const int& number)
	{
		// ������ �ִϸ��̼� �ؽ�Ʈ ( ex. Animation00 ... Animation99 )
		std::string animationName;
		animationName = "Animation Clip" + std::to_string(number) + "(Frame : " + std::to_string(mAnimationClipContainer[number][0].size()) + ")";

		if (ImGui::TreeNode(animationName.c_str()))
		{
			beginButton_AnimationPlay(number);
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

	void PhysicsAnimatorWindow::beginButton_AnimationSave(const int& number)
	{
		// ������ �ִϸ��̼� ���̺� ( ���̺� ��ư ������ �ش� �ִϸ��̼� �����͸� ���� )
		if (ImGui::Button("Save"))
		{

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
				mExtractObjects.clear();
				mAnimationClipContainer.clear();

				mRegisteredObject = static_cast<fq::game_module::GameObject*>(PathPayLoad->Data);

				std::vector<fq::game_module::GameObject*> gameObjects = mRegisteredObject->GetChildren();
				mExtractObjects.reserve(gameObjects.size());
				mExtractObjects.push_back(mRegisteredObject);
				for (auto& object : gameObjects)
				{
					if (object->HasComponent<fq::game_module::SkinnedMeshRenderer>() || object->HasComponent<fq::game_module::StaticMeshRenderer>())
					{
						if (!object->HasComponent<fq::game_module::RigidBody>())
							break;

						mExtractObjects.push_back(object);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}


		int a = 0;

	}
}
