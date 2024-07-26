#include "CharacterJoint.h"

#include "CharacterLink.h"
#include "EngineDataConverter.h"


namespace fq::physics
{
	constexpr float CircularMeasure = 0.01744f;

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

	bool CharacterJoint::Initialize(const std::shared_ptr<CharacterLink> parentLink, const std::shared_ptr<CharacterLink> ownerLink, const JointInfo& info)
	{
		mParentLink = parentLink;
		mOwnerLink = ownerLink;
		mLocalTransform = info.localTransform;
		mSwing1Limit.high = info.Swing1AxisInfo.limitsHigh;
		mSwing1Limit.low = info.Swing1AxisInfo.limitsLow;
		mSwing2Limit.high = info.Swing2AxisInfo.limitsHigh;
		mSwing2Limit.low = info.Swing2AxisInfo.limitsLow;
		mTwistLimit.high = info.TwistAxisInfo.limitsHigh;
		mTwistLimit.low = info.TwistAxisInfo.limitsLow;
		mDrive.damping = info.damping;
		mDrive.maxForce = info.maxForce;
		mDrive.stiffness = info.stiffness;
		mDrive.driveType = physx::PxArticulationDriveType::eFORCE;

		mPxJoint = ownerLink->GetPxLink()->getInboundJoint();
		mPxJoint->setMaxJointVelocity(5.f);
		mPxJoint->setFrictionCoefficient(0.1f);

		physx::PxTransform pxlocalTransform;
		DirectX::SimpleMath::Matrix parentLocalTransform = mLocalTransform * ownerLink->GetLocalTransform();

		CopyDirectXMatrixToPxTransform(mLocalTransform, pxlocalTransform);
		mPxJoint->setChildPose(pxlocalTransform);

		CopyDirectXMatrixToPxTransform(parentLocalTransform, pxlocalTransform);
		mPxJoint->setParentPose(pxlocalTransform);

		if (info.Swing1AxisInfo.motion == EArticulationMotion::LOCKED && info.Swing2AxisInfo.motion == EArticulationMotion::LOCKED && info.TwistAxisInfo.motion == EArticulationMotion::LOCKED)
			mPxJoint->setJointType(physx::PxArticulationJointType::eFIX);
		else
			mPxJoint->setJointType(physx::PxArticulationJointType::eSPHERICAL);

		mPxJoint->setMotion(physx::PxArticulationAxis::eSWING1, (physx::PxArticulationMotion::Enum)info.Swing1AxisInfo.motion);
		mPxJoint->setMotion(physx::PxArticulationAxis::eSWING2, (physx::PxArticulationMotion::Enum)info.Swing2AxisInfo.motion);
		mPxJoint->setMotion(physx::PxArticulationAxis::eTWIST, (physx::PxArticulationMotion::Enum)info.TwistAxisInfo.motion);

		physx::PxArticulationLimit swing1Limit(mSwing1Limit.low * CircularMeasure, mSwing1Limit.high * CircularMeasure);
		physx::PxArticulationLimit swing2Limit(mSwing2Limit.low * CircularMeasure, mSwing2Limit.high * CircularMeasure);
		physx::PxArticulationLimit twistLimit(mTwistLimit.low * CircularMeasure, mTwistLimit.high * CircularMeasure);

		mPxJoint->setLimitParams(physx::PxArticulationAxis::eSWING1, swing1Limit);
		mPxJoint->setLimitParams(physx::PxArticulationAxis::eSWING2, swing2Limit);
		mPxJoint->setLimitParams(physx::PxArticulationAxis::eTWIST, twistLimit);

		mPxJoint->setDriveParams(physx::PxArticulationAxis::eSWING1, mDrive);
		mPxJoint->setDriveParams(physx::PxArticulationAxis::eSWING2, mDrive);
		mPxJoint->setDriveParams(physx::PxArticulationAxis::eTWIST, mDrive);

		return true;
	}

	bool CharacterJoint::Update(const physx::PxArticulationLink* parentLink)
	{
		if (!mPxJoint)
			return true;

		physx::PxArticulationLink& childLink = mPxJoint->getChildArticulationLink();

		physx::PxTransform parentJointLocalPoseInChild;

		// 부모 Actor의 Joint의 LocalPose를 가져옵니다.
		if (parentLink->getInboundJoint())
		{
			parentJointLocalPoseInChild = parentLink->getInboundJoint()->getChildPose();
		}
		else
		{
			parentJointLocalPoseInChild = physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f));
		}

		// 부모와 자식 Actor의 GlobalPose를 가져옵니다.
		physx::PxTransform parentGlobalPose = parentLink->getGlobalPose();
		physx::PxTransform childGlobalPose = childLink.getGlobalPose();

		// joint의 child에 대한 로컬 포즈를 가져옵니다.
		physx::PxTransform jointLocalPoseInChild = mPxJoint->getChildPose();

		// 자식의 LocalPose를 GlobalPose로 변환합니다.
		physx::PxTransform parentJointGlobalPose = parentGlobalPose * parentJointLocalPoseInChild;
		physx::PxTransform jointGlobalPose = childGlobalPose * jointLocalPoseInChild;

		// 부모 joint와 자식 joint를 DxMatrix로 변환합니다.
		DirectX::SimpleMath::Matrix dxParentJointGlobalTransform;
		DirectX::SimpleMath::Matrix dxChildJointGlobalTransform;
		CopyPxTransformToDirectXMatrix(parentJointGlobalPose, dxParentJointGlobalTransform);
		CopyPxTransformToDirectXMatrix(jointGlobalPose, dxChildJointGlobalTransform);

		// 부모 joint의 인버트 함수를 활용하여 자식 joint의 로컬 트랜스폼을 계산합니다.
		mSimulationLocalTransform = dxChildJointGlobalTransform * dxParentJointGlobalTransform.Invert();

		mSimulationLocalTransform = mSimulationLocalTransform;

		return true;
	}
}
