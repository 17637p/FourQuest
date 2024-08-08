#include "ObjectTeleportTrack.h"

#include "Transform.h"
#include "Scene.h"

namespace fq::game_module
{
	ObjectTeleportTrack::ObjectTeleportTrack()
		: Track(ETrackType::OBJECT_TELEPORT)
		, mTargetObject()
		, mTargetPosition{}
		, mTargetRotation{}
		, mTargetScale{}
	{
	}
	ObjectTeleportTrack::~ObjectTeleportTrack()
	{
	}

	bool ObjectTeleportTrack::Initialize(ObjectTeleportTrackInfo info, Scene* scene)
	{
		mStartTime = info.startTime;
		mTotalPlayTime = info.totalPlayTime;

		mTargetObject = scene->GetObjectByName(info.targetObjectName);

		if (mTargetObject.expired())
			return false;

		mTargetPosition = info.targetPosition;
		mTargetRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(info.targetRotation);
		mTargetScale = info.targetScale;

		auto transform = mTargetObject.lock()->GetComponent<Transform>();
		mPrevPosition = transform->GetWorldPosition();
		mPrevRotation = transform->GetWorldRotation();
		mPrevScale = transform->GetWorldScale();

		return true;
	}

	void ObjectTeleportTrack::PlayEnter()
	{
		if (!mTargetObject.expired())
		{
			auto transform = mTargetObject.lock()->GetComponent<Transform>();

			DirectX::SimpleMath::Matrix teleportTransform =
				DirectX::SimpleMath::Matrix::CreateScale(mTargetScale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(mTargetRotation)
				* DirectX::SimpleMath::Matrix::CreateTranslation(mTargetPosition);

			transform->SetWorldMatrix(teleportTransform);
		}
	}
	void ObjectTeleportTrack::PlayOn()
	{
	}
	void ObjectTeleportTrack::PlayExit()
	{
	}
	void ObjectTeleportTrack::End()
	{
		if (!mTargetObject.expired())
		{
			auto transform = mTargetObject.lock()->GetComponent<Transform>();

			DirectX::SimpleMath::Matrix prevTransform =
				DirectX::SimpleMath::Matrix::CreateScale(mPrevScale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(mPrevRotation)
				* DirectX::SimpleMath::Matrix::CreateTranslation(mPrevPosition);

			transform->SetWorldMatrix(prevTransform);
		}
	}
}
