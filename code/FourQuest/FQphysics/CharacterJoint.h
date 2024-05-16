#pragma once

#include <directxtk\SimpleMath.h>
#include <physx\PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterLink;

	class CharacterJoint
	{
	public:
		CharacterJoint();
		~CharacterJoint();

		bool Initialize(CharacterLink* parentLink, CharacterLink* ownerLink, const CharacterJointInfo& info);

	private:
		CharacterLink* mOwnerLink;
		CharacterLink* mParentLink;

		DirectX::SimpleMath::Matrix mLocalTransform;

	private:
		physx::PxArticulationJointReducedCoordinate* mPxJoint;

		physx::PxArticulationDrive mDrive;
		physx::PxArticulationLimit mSwing1Limit;
		physx::PxArticulationLimit mSwing2Limit;
		physx::PxArticulationLimit mTwistLimit;
	};
}