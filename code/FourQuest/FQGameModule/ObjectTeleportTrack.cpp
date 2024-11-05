#include "ObjectTeleportTrack.h"

#include "Transform.h"
#include "Scene.h"

#include <spdlog/spdlog.h>

namespace fq::game_module
{
	ObjectTeleportTrack::ObjectTeleportTrack()
		: Track(ETrackType::OBJECT_TELEPORT)
		, mbIsObjectReturnToStartTransform()
		, mTargetObject()
		, mKeys{}
		, mPrevPosition{}
		, mPrevRotation{}
		, mPrevScale{}
	{
	}
	ObjectTeleportTrack::~ObjectTeleportTrack()
	{
	}

	bool ObjectTeleportTrack::Initialize(const ObjectTeleportTrackInfo& info, Scene* scene)
	{
		mScene = scene;
		mTrackObjectName.push_back(info.targetObjectName);
		mStartTime = info.startTime;
		mTargetObjectName = info.targetObjectName;
		mTotalPlayTime = info.totalPlayTime;
		mbIsObjectReturnToStartTransform = info.isObjectReturnToStartTransform;

		mKeys = info.keys;


		return true;
	}

	void ObjectTeleportTrack::PlayEnter()
	{
		mTargetObject = mScene->GetObjectByName(mTargetObjectName);

		// 해당 오브젝트가 존재하지 않으면 로그 띄우기
		if (mTargetObject == nullptr)
		{
			spdlog::warn("[ObjectTeleportTrack Warrning ({})] Do not Have TargetObject {}", __LINE__, mTargetObjectName);
			return;
		}

		if (!mTargetObject->HasComponent<Transform>())
		{
			spdlog::warn("[ObjectTeleportTrack Warrning ({})] TargetObject Have not Trasfrom Component", __LINE__);
			return;
		}

		auto transform = mTargetObject->GetComponent<Transform>();
		mPrevPosition = transform->GetWorldPosition();
		mPrevRotation = transform->GetWorldRotation();
		mPrevScale = transform->GetWorldScale();

		// time 값에 따라 TrackKey 벡터를 오름차순으로 정렬
		std::sort(mKeys.begin(), mKeys.end(), [](const TrackKey& a, const TrackKey& b)
			{
				return a.time < b.time;
			});
	}

	void ObjectTeleportTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;

		if (mTargetObject == nullptr)
		{
			spdlog::warn("Object not found, Name : {}", mTargetObjectName);
			assert(mTargetObject != nullptr);
			return;
		}

		if (!mTargetObject->IsDestroyed())
		{
			if (!mTargetObject->HasComponent<Transform>()) return;

			auto transform = mTargetObject->GetComponent<Transform>();

			for (int i = 0; i < mKeys.size(); i++)
			{
				if (mElapsedTime >= mKeys[i].time && checkPointTime < mKeys[i].time)
				{
					checkPointTime = mKeys[i].time;
					keyNumber = i;
				}
			}

			DirectX::SimpleMath::Matrix teleportTransform =
				DirectX::SimpleMath::Matrix::CreateScale(mKeys[keyNumber].scale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(mKeys[keyNumber].rotation / 180.f * 3.14f))
				* DirectX::SimpleMath::Matrix::CreateTranslation(mKeys[keyNumber].position);

			transform->SetWorldMatrix(teleportTransform);
		}
	}

	void ObjectTeleportTrack::PlayExit()
	{
	}
	void ObjectTeleportTrack::End()
	{
		// 시퀀스가 시작하기 이전 Transform로 돌아가야 할 경우
		if (mTargetObject && !mTargetObject->IsDestroyed() && mbIsObjectReturnToStartTransform)
		{
			if (!mTargetObject->HasComponent<Transform>()) return;

			auto transform = mTargetObject->GetComponent<Transform>();

			DirectX::SimpleMath::Matrix prevTransform =
				DirectX::SimpleMath::Matrix::CreateScale(mPrevScale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(mPrevRotation)
				* DirectX::SimpleMath::Matrix::CreateTranslation(mPrevPosition);

			transform->SetWorldMatrix(prevTransform);
		}
	}
}
