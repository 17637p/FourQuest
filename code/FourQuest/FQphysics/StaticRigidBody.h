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
		/// ������ �ٵ� ������ �����͸� �޾Ƽ� ������ �ٵ� �����մϴ�.
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