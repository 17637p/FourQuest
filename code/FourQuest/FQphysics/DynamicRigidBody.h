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
		/// ������ �ٵ� ������ �����͸� �޾Ƽ� ������ �ٵ� �����մϴ�.
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