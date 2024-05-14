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

		// 이벤트 핸들 등록
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
			// 애니메이터 창
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
		// 데이터를 추출할 키 프레임 사이즈 입력
		ImGui::InputInt("KeyFrameSize (1Second = 30 FPS)", &mKeyFrameSize);
	}

	void PhysicsAnimatorWindow::beginText_ObjectName()
	{
		ImGui::Text(mRegisteredObject->GetName().c_str());
	}

	void PhysicsAnimatorWindow::beginTree_Animation(const int& number)
	{
		// 추출한 애니메이션 텍스트 ( ex. Animation00 ... Animation99 )
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
		// 추출한 애니메이션 플레이 ( 플레이 버튼 누르면 업데이트 되면서 해당 메시 컴포넌트들에게 setTransform을 통해 애니메이션 플레이 )
		if (ImGui::Button("Play"))
		{
			mPlayAnimationClipNumber = number;
			mDurationTime = 0.f;
			mbIsPlay = true;
		}
	}

	void PhysicsAnimatorWindow::beginButton_AnimationSave(const int& number)
	{
		// 추출한 애니메이션 세이브 ( 세이브 버튼 누르면 해당 애니메이션 데이터를 저장 )
		if (ImGui::Button("Save"))
		{

		}
	}

	void PhysicsAnimatorWindow::dragDropGameObject()
	{
		// Drop 처리
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
