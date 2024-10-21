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
	{
	}
	ObjectAnimationTrack::~ObjectAnimationTrack()
	{
	}

	bool ObjectAnimationTrack::Initialize(const ObjectAnimationInfo& info, Scene* scene, std::vector<std::shared_ptr<fq::graphics::IAnimation>> animationContainer)
	{
		mTrackObjectName.push_back(info.targetObjectName);
		mAnimationContainer = animationContainer;

		mbIsLoop = info.isLoop;
		mTotalPlayTime = info.totalPlayTime;
		mStartTime = info.startTime;
		mTargetObjectName = info.targetObjectName;
		mAnimationTrackKeys = info.animationTrackKeys;

		mTargetObject = scene->GetObjectByName(info.targetObjectName);

		// 해당 오브젝트가 존재하지 않으면 로그 띄우기
		if (mTargetObject.expired())
		{
			spdlog::warn("[Warrning] Do not Have TargetObject");
			return false;
		}

		// 해당 오브젝트가 Transform을 가지고 있지 않으면 로그 띄우기
		if (!mTargetObject.lock()->HasComponent<Transform>())
		{
			spdlog::warn("[Warrning] TargetObject Have not Trasfrom Component");
			return false;
		}

		return true;
	}

	void ObjectAnimationTrack::PlayEnter()
	{
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

		if (!mTargetObject.expired())
		{
			if (!mTargetObject.lock()->HasComponent<Animator>()) return;

			auto animator = mTargetObject.lock()->GetComponent<Animator>();
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
				float animationTotalTime = mAnimationContainer[keyNumber]->GetAnimationClip().Duration;

				while (time >= animationTotalTime / mAnimationTrackKeys[keyNumber].speed)
					time -= animationTotalTime / mAnimationTrackKeys[keyNumber].speed;
			}

			nodeHierarchyInstance.Update(time * mAnimationTrackKeys[keyNumber].speed, mAnimationContainer[keyNumber]);
		}
	}

	void ObjectAnimationTrack::PlayExit()
	{
		if (!mTargetObject.expired())
		{
			if (!mTargetObject.lock()->HasComponent<Animator>()) return;

			auto animator = mTargetObject.lock()->GetComponent<Animator>();

			auto& nodeHierarchyInstance = animator->GetNodeHierarchyInstance();

			nodeHierarchyInstance.SetBindPose();
			nodeHierarchyInstance.UpdateByLocalTransform();
		}
	}

	void ObjectAnimationTrack::End()
	{
		if (!mTargetObject.expired())
		{
			if (!mTargetObject.lock()->HasComponent<Animator>()) return;

			auto animator = mTargetObject.lock()->GetComponent<Animator>();

			auto& nodeHierarchyInstance = animator->GetNodeHierarchyInstance();

			nodeHierarchyInstance.SetBindPose();
			nodeHierarchyInstance.UpdateByLocalTransform();
		}
	}
}
