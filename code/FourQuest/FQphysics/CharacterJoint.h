#pragma once

#include <memory>
#include <directxtk\SimpleMath.h>
#include <PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterLink;

	class CharacterJoint
	{
	public:
		CharacterJoint();
		~CharacterJoint();

		/// <summary>
		/// 캐릭터 조인트 초기화 함수
		/// </summary>
		bool Initialize(const std::shared_ptr<CharacterLink> parentLink, const std::shared_ptr<CharacterLink> ownerLink, const JointInfo& info);

		/// <summary>
		/// 업데이트
		/// </summary>
		bool Update(const physx::PxArticulationLink* parentLink);
		
		inline const std::shared_ptr<CharacterLink> GetOwnerLink();
		inline const std::shared_ptr<CharacterLink> GetParentLink();
		inline const DirectX::SimpleMath::Matrix& GetLocalTransform();
		inline const DirectX::SimpleMath::Matrix& GetSimulationTransform();
		inline const DirectX::SimpleMath::Matrix& GetSimulationOffsetTransform();
		inline const DirectX::SimpleMath::Matrix& GetSimulationWorldTransform();
		inline const physx::PxArticulationJointReducedCoordinate* GetPxJoint();
		inline const physx::PxArticulationDrive& GetDrive();
		inline const physx::PxArticulationLimit& GetSwing1Limit();
		inline const physx::PxArticulationLimit& GetSwing2Limit();
		inline const physx::PxArticulationLimit& GetTwistLimit();

	private:
		std::weak_ptr<CharacterLink> mOwnerLink;
		std::weak_ptr<CharacterLink> mParentLink;

		DirectX::SimpleMath::Matrix mLocalTransform;
		DirectX::SimpleMath::Matrix mSimulationOffsetTransform;
		DirectX::SimpleMath::Matrix mSimulationLocalTransform;
		DirectX::SimpleMath::Matrix mSimulationWorldTransform;

	private:
		physx::PxArticulationJointReducedCoordinate* mPxJoint;

		physx::PxArticulationDrive mDrive;
		physx::PxArticulationLimit mSwing1Limit;
		physx::PxArticulationLimit mSwing2Limit;
		physx::PxArticulationLimit mTwistLimit;
	};

#pragma region GetSet

	const std::shared_ptr<CharacterLink> CharacterJoint::GetOwnerLink()
	{
		return mOwnerLink.lock();
	}
	const std::shared_ptr<CharacterLink> CharacterJoint::GetParentLink()
	{
		return mParentLink.lock();
	}
	const DirectX::SimpleMath::Matrix& CharacterJoint::GetLocalTransform()
	{
		return mLocalTransform;
	}
	const DirectX::SimpleMath::Matrix& CharacterJoint::GetSimulationTransform()
	{
		return mSimulationLocalTransform;
	}
	const DirectX::SimpleMath::Matrix& CharacterJoint::GetSimulationOffsetTransform()
	{
		return mSimulationOffsetTransform;
	}
	const DirectX::SimpleMath::Matrix& CharacterJoint::GetSimulationWorldTransform()
	{
		return mSimulationWorldTransform;
	}
	const physx::PxArticulationJointReducedCoordinate* CharacterJoint::GetPxJoint()
	{
		return mPxJoint;
	}
	const physx::PxArticulationDrive& CharacterJoint::GetDrive()
	{
		return mDrive;
	}
	const physx::PxArticulationLimit& CharacterJoint::GetSwing1Limit()
	{
		return mSwing1Limit;
	}
	const physx::PxArticulationLimit& CharacterJoint::GetSwing2Limit()
	{
		return mSwing2Limit;
	}
	const physx::PxArticulationLimit& CharacterJoint::GetTwistLimit()
	{
		return mTwistLimit;
	}
#pragma endregion
}