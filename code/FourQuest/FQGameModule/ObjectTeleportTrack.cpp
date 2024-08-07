#include "ObjectTeleportTrack.h"

#include "Transform.h"

namespace fq::game_module
{
	ObjectTeleportTrack::ObjectTeleportTrack(ETrackType type)
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

	void ObjectTeleportTrack::PlayEnter()
	{
		if (mTargetObject.expired())
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
}
