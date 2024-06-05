#include "RigidBody.h"

namespace fq::physics
{
	RigidBody::RigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber)
		: mColliderType(colliderType)
		, mID(id)
		, mLayerNumber(layerNumber)
		, mRadius()
		, mHalfHeight()
		, mExtent()
	{
	}

	RigidBody::~RigidBody()
	{
	}

	void RigidBody::updateShapeGeometry(physx::PxRigidActor* actor, const physx::PxGeometry& newGeometry, physx::PxPhysics* physics, physx::PxMaterial* material)
	{
		// ���ο� shape ����
		physx::PxShape* newShape = physics->createShape(newGeometry, *material);

		// ���ο� shape�� actor�� �߰�
		actor->attachShape(*newShape);

		// ���ο� shape ����
		newShape->release();
	}
}
