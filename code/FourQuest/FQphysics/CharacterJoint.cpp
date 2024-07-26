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

		// �θ� Actor�� Joint�� LocalPose�� �����ɴϴ�.
		if (parentLink->getInboundJoint())
		{
			parentJointLocalPoseInChild = parentLink->getInboundJoint()->getChildPose();
		}
		else
		{
			parentJointLocalPoseInChild = physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f));
		}

		// �θ�� �ڽ� Actor�� GlobalPose�� �����ɴϴ�.
		physx::PxTransform parentGlobalPose = parentLink->getGlobalPose();
		physx::PxTransform childGlobalPose = childLink.getGlobalPose();

		// joint�� child�� ���� ���� ��� �����ɴϴ�.
		physx::PxTransform jointLocalPoseInChild = mPxJoint->getChildPose();

		// �ڽ��� LocalPose�� GlobalPose�� ��ȯ�մϴ�.
		physx::PxTransform parentJointGlobalPose = parentGlobalPose * parentJointLocalPoseInChild;
		physx::PxTransform jointGlobalPose = childGlobalPose * jointLocalPoseInChild;

		// �θ� joint�� �ڽ� joint�� DxMatrix�� ��ȯ�մϴ�.
		DirectX::SimpleMath::Matrix dxParentJointGlobalTransform;
		DirectX::SimpleMath::Matrix dxChildJointGlobalTransform;
		CopyPxTransformToDirectXMatrix(parentJointGlobalPose, dxParentJointGlobalTransform);
		CopyPxTransformToDirectXMatrix(jointGlobalPose, dxChildJointGlobalTransform);

		// �θ� joint�� �ι�Ʈ �Լ��� Ȱ���Ͽ� �ڽ� joint�� ���� Ʈ�������� ����մϴ�.
		mSimulationLocalTransform = dxChildJointGlobalTransform * dxParentJointGlobalTransform.Invert();

		mSimulationLocalTransform = mSimulationLocalTransform;

		return true;
	}
}
