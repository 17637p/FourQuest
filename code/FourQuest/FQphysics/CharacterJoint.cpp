#include "CharacterJoint.h"

#include "CharacterLink.h"

namespace fq::physics
{
	CharacterJoint::CharacterJoint()
		: mOwnerLink()
		, mParentLink()
		, mLocalTransform()
		, mPxJoint(nullptr)
		, mDrive()
		, mSwing1Limit()
		, mSwing2Limit()
		, mTwistLimit()
	{
	}

	CharacterJoint::~CharacterJoint()
	{
	}

	bool CharacterJoint::Initialize(const std::shared_ptr<CharacterLink> parentLink, const std::shared_ptr<CharacterLink> ownerLink, const CharacterJointInfo& info)
	{
		mParentLink = parentLink;
		mOwnerLink = ownerLink;
		mLocalTransform = info.localTransform;
		mSwing1Limit.high = info.jointSwing1AxisInfo.limitsHigh;
		mSwing1Limit.low = info.jointSwing1AxisInfo.limitsLow;
		mSwing2Limit.high = info.jointSwing2AxisInfo.limitsHigh;
		mSwing2Limit.low = info.jointSwing2AxisInfo.limitsLow;
		mTwistLimit.high = info.jointTwistAxisInfo.limitsHigh;
		mTwistLimit.low = info.jointTwistAxisInfo.limitsLow;
		mDrive.damping = info.damping;
		mDrive.maxForce = info.maxForce;
		mDrive.stiffness = info.stiffness;
		mDrive.driveType = physx::PxArticulationDriveType::eFORCE;

		mPxJoint = ownerLink->GetPxLink()->getInboundJoint();
		mPxJoint->setJointType(physx::PxArticulationJointType::eSPHERICAL);
		mPxJoint->setMotion(physx::PxArticulationAxis::eSWING1, (physx::PxArticulationMotion::Enum)info.jointSwing1AxisInfo.motion);
		mPxJoint->setMotion(physx::PxArticulationAxis::eSWING2, (physx::PxArticulationMotion::Enum)info.jointSwing2AxisInfo.motion);
		mPxJoint->setMotion(physx::PxArticulationAxis::eTWIST, (physx::PxArticulationMotion::Enum)info.jointTwistAxisInfo.motion);

		mPxJoint->setLimitParams(physx::PxArticulationAxis::eSWING1, mSwing1Limit);
		mPxJoint->setLimitParams(physx::PxArticulationAxis::eSWING2, mSwing2Limit);
		mPxJoint->setLimitParams(physx::PxArticulationAxis::eTWIST, mTwistLimit);

		mPxJoint->setDriveParams(physx::PxArticulationAxis::eSWING1, mDrive);
		mPxJoint->setDriveParams(physx::PxArticulationAxis::eSWING2, mDrive);
		mPxJoint->setDriveParams(physx::PxArticulationAxis::eTWIST, mDrive);

		return true;
	}
}
