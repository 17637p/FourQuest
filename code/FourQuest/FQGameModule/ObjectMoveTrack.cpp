#include "ObjectMoveTrack.h"

#include "Scene.h"
#include "Transform.h"

DirectX::SimpleMath::Vector3 LerpVector3(const DirectX::SimpleMath::Vector3& a, const DirectX::SimpleMath::Vector3& b, float t)
{
	return DirectX::SimpleMath::Vector3(
		std::lerp(a.x, b.x, t),
		std::lerp(a.y, b.y, t),
		std::lerp(a.z, b.z, t)
	);
}
DirectX::SimpleMath::Quaternion LerpQuaternion(const DirectX::SimpleMath::Quaternion& a, const DirectX::SimpleMath::Quaternion& b, float t)
{
	return DirectX::SimpleMath::Quaternion(
		std::lerp(a.x, b.x, t),
		std::lerp(a.y, b.y, t),
		std::lerp(a.z, b.z, t),
		std::lerp(a.w, b.w, t)
	);
}

namespace fq::game_module
{
	ObjectMoveTrack::ObjectMoveTrack()
		: Track(ETrackType::OBJECT_MOVE)
		, mTargetObject()
		, mTargetPosition{}
		, mTargetRotation{}
		, mTargetScale{}
	{
	}
	ObjectMoveTrack::~ObjectMoveTrack()
	{
	}

	bool ObjectMoveTrack::Initialize(ObjectMoveTrackInfo info, Scene* scene)
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

	void ObjectMoveTrack::PlayEnter()
	{
	}

	void ObjectMoveTrack::PlayOn()
	{
		if (!mTargetObject.expired())
		{
			if (!mTargetObject.lock()->HasComponent<Transform>()) return;

			auto transform = mTargetObject.lock()->GetComponent<Transform>();

			float lerpValue = mElapsedTime / mTotalPlayTime;
			DirectX::SimpleMath::Vector3 scale = LerpVector3(mPrevScale, mTargetScale, lerpValue);
			DirectX::SimpleMath::Quaternion rotation = LerpQuaternion(mPrevRotation, mTargetRotation, lerpValue);
			DirectX::SimpleMath::Vector3 position = LerpVector3(mPrevPosition, mTargetPosition, lerpValue);

			DirectX::SimpleMath::Matrix newTransform =
				DirectX::SimpleMath::Matrix::CreateScale(scale)
				* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation)
				* DirectX::SimpleMath::Matrix::CreateTranslation(position);

			transform->SetWorldMatrix(newTransform);
		}
	}

	void ObjectMoveTrack::PlayExit()
	{
	}
	void ObjectMoveTrack::End()
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
