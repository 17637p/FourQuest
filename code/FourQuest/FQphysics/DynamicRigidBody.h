#pragma once

#include "RigidBody.h"

namespace fq::physics
{
	class DynamicRigidBody : public RigidBody
	{
	public:
		DynamicRigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber);
		virtual ~DynamicRigidBody();

		/// <summary>
		/// 리지드 바디를 생성할 데이터를 받아서 리지드 바디를 생성합니다.
		/// </summary>
		bool Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics);

		inline physx::PxRigidDynamic* GetRigidDynamic();

	private:
		physx::PxRigidDynamic* mRigidDynamic;
	};

	physx::PxRigidDynamic* DynamicRigidBody::GetRigidDynamic()
	{
		return mRigidDynamic;
	}
}