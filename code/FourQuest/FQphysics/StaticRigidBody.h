#pragma once

#include "RigidBody.h"

namespace fq::physics
{
	class StaticRigidBody : public RigidBody
	{
	public:
		StaticRigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber);
		virtual ~StaticRigidBody();

		/// <summary>
		/// 리지드 바디를 생성할 데이터를 받아서 리지드 바디를 생성합니다.
		/// </summary>
		bool Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics);

		inline physx::PxRigidStatic* GetRigidStatic();

	private:
		physx::PxRigidStatic* mRigidStatic;
	};

	physx::PxRigidStatic* StaticRigidBody::GetRigidStatic()
	{
		return mRigidStatic;
	}
}