#include "ObjectAnimationTrack.h"

#include "Scene.h"
#include "Animator.h"
#include "AnimatorController.h"
#include "Transform.h"

namespace fq::game_module
{
	ObjectAnimationTrack::ObjectAnimationTrack()
		: Track(ETrackType::ANIMAITON)
		, mTargetObjectName{}
		, mAnimationTrackKeys{}
		, mAnimationContainer{}
		, mTargetObject()
		, mbIsLoop(true)
		, mScene(nullptr)
	{
	}
	ObjectAnimationTrack::~ObjectAnimationTrack()
	{
	}

	bool ObjectAnimationTrack::Initialize(const ObjectAnimationInfo& info, Scene* scene, std::vector<std::shared_ptr<fq::graphics::IAnimation>> animationContainer)
	{
		mScene = scene;

		mTrackObjectName.push_back(info.targetObjectName);
		mAnimationContainer = animationContainer;

		mbIsLoop = info.isLoop;
		mTotalPlayTime = info.totalPlayTime;
		mStartTime = info.startTime;
		mTargetObjectName = info.targetObjectName;
		mAnimationTrackKeys = info.animationTrackKeys;

		return true;
	}

	void ObjectAnimationTrack::PlayEnter()
	{
		mTargetObject = mScene->GetObjectByName(mTargetObjectName);

		// 해당 오브젝트가 존재하지 않으면 로그 띄우기
		if (mTargetObject == nullptr)
		{
			spdlog::warn("[ObjectAnimationTrack Warrning({})]  Do not Have TargetObject Name [{}] ", __LINE__ , mTargetObjectName);
		}
		else if (!mTargetObject->HasComponent<Transform>())
		{
			spdlog::warn("[ObjectAnimationTrack Warrning({})] TargetObject Have not Trasfrom Component", __LINE__);
		}

		// time 값에 따라 TrackKey 벡터를 오름차순으로 정렬
		std::sort(mAnimationTrackKeys.begin(), mAnimationTrackKeys.end(), [](const AnimationTrackKey& a, const AnimationTrackKey& b)
			{
				return a.time < b.time;
			});
	}

	void ObjectAnimationTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			if (!mTargetObject->HasComponent<Animator>()) return;

			auto animator = mTargetObject->GetComponent<Animator>();
			animator->SetStopAnimation(true);

			for (int i = 0; i < mAnimationTrackKeys.size(); i++)
			{
				if (mElapsedTime >= mAnimationTrackKeys[i].time && checkPointTime < mAnimationTrackKeys[i].time)
				{
					checkPointTime = mAnimationTrackKeys[i].time;
					keyNumber = i;
				}
			}

			auto& nodeHierarchyInstance = animator->GetNodeHierarchyInstance();

			float time = mElapsedTime - checkPointTime;

			if (mbIsLoop)
			{
				if (keyNumber >= mAnimationContainer.size())
				{
					spdlog::warn("[ObjectAnimationTrack ({})] Animation KeyNumber Over the Animation Key Size", __LINE__);
				}

				float animationTotalTime = mAnimationContainer[keyNumber]->GetAnimationClip().Duration;

				while (time >= animationTotalTime / mAnimationTrackKeys[keyNumber].speed)
				{
					time -= animationTotalTime / mAnimationTrackKeys[keyNumber].speed;

					if (time < 0)
					{
						spdlog::warn("[ObjectAnimationTrack ({})] Animation Time is Minus", __LINE__);
						return;
					}
				}
			}

			nodeHierarchyInstance.Update(time * mAnimationTrackKeys[keyNumber].speed, mAnimationContainer[keyNumber]);
		}
	}

	void ObjectAnimationTrack::PlayExit()
	{
		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			if (!mTargetObject->HasComponent<Animator>()) return;

			auto animator = mTargetObject->GetComponent<Animator>();

			auto& nodeHierarchyInstance = animator->GetNodeHierarchyInstance();

			nodeHierarchyInstance.SetBindPose();
			nodeHierarchyInstance.UpdateByLocalTransform();
		}
	}

	void ObjectAnimationTrack::End()
	{
		if (mTargetObject && !mTargetObject->IsDestroyed())
		{
			if (!mTargetObject->HasComponent<Animator>()) return;

			auto animator = mTargetObject->GetComponent<Animator>();

			auto& nodeHierarchyInstance = animator->GetNodeHierarchyInstance();

			nodeHierarchyInstance.SetBindPose();
			nodeHierarchyInstance.UpdateByLocalTransform();
		}
	}
}
