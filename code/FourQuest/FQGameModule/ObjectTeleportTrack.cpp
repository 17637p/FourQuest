#include "ObjectTeleportTrack.h"

#include "Transform.h"
#include "Scene.h"

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
		mTrackObjectName.push_back(info.targetObjectName);
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;
		mbIsObjectReturnToStartTransform = info.isObjectReturnToStartTransform;

		mTargetObject = scene->GetObjectByName(info.targetObjectName);

		// �ش� ������Ʈ�� �������� ������ �α� ����
		if (mTargetObject.expired())
		{
			spdlog::warn("[ObjectTeleportTrack Warrning ({})] Do not Have TargetObject", __LINE__);
			return false;
		}

		if (!mTargetObject.lock()->HasComponent<Transform>()) 
		{
			spdlog::warn("[ObjectTeleportTrack Warrning ({})] TargetObject Have not Trasfrom Component", __LINE__);
			return false;
		}

		mKeys = info.keys;

		auto transform = mTargetObject.lock()->GetComponent<Transform>();
		mPrevPosition = transform->GetWorldPosition();
		mPrevRotation = transform->GetWorldRotation();
		mPrevScale = transform->GetWorldScale();

		return true;
	}

	void ObjectTeleportTrack::PlayEnter()
	{
		// time ���� ���� TrackKey ���͸� ������������ ����
		std::sort(mKeys.begin(), mKeys.end(), [](const TrackKey& a, const TrackKey& b)
			{
				return a.time < b.time;
			});
	}

	void ObjectTeleportTrack::PlayOn()
	{
		int keyNumber = 0;
		float checkPointTime = 0.f;


		if (!mTargetObject.expired())
		{
			if (!mTargetObject.lock()->HasComponent<Transform>()) return;

			auto transform = mTargetObject.lock()->GetComponent<Transform>();

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
		// �������� �����ϱ� ���� Transform�� ���ư��� �� ���
		if (!mTargetObject.expired() && mbIsObjectReturnToStartTransform)
		{
			if (!mTargetObject.lock()->HasComponent<Transform>()) return;

			auto transform = mTargetObject.lock()->GetComponent<Transform>();

			DirectX::SimpleMath::Matrix prevTransform =
				DirectX::SimpleMath::Matrix::CreateScale(mPrevScale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(mPrevRotation)
				* DirectX::SimpleMath::Matrix::CreateTranslation(mPrevPosition);

			transform->SetWorldMatrix(prevTransform);
		}
	}
}
