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
		// 货肺款 shape 积己
		physx::PxShape* newShape = physics->createShape(newGeometry, *material);

		// 货肺款 shape甫 actor俊 眠啊
		actor->attachShape(*newShape);

		// 货肺款 shape 秦力
		newShape->release();
	}
}
