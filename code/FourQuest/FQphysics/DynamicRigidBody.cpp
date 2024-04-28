#include "DynamicRigidBody.h"

namespace fq::physics
{
	DynamicRigidBody::DynamicRigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber)
		: RigidBody(colliderType, id, layerNumber)
		, mRigidDynamic(nullptr)
	{
	}
	DynamicRigidBody::~DynamicRigidBody()
	{
	}
	bool DynamicRigidBody::Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics)
	{
		if (GetColliderType() == EColliderType::COLLISION)
		{
			shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		}
		else
		{
			shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
		}

		CollisionData* data = new CollisionData;
		data->myId = GetID();
		data->myLayerNumber = GetLayerNumber();
		shape->userData = data;

		physx::PxMat44 matrix;
		memcpy(&matrix, &colliderInfo.collisionTransform.worldMatrix, sizeof(physx::PxMat44));
		physx::PxTransform transform(matrix);

		mRigidDynamic = physics->createRigidDynamic(transform);
		if (!mRigidDynamic->attachShape(*shape))
			return false;
		physx::PxRigidBodyExt::updateMassAndInertia(*mRigidDynamic, colliderInfo.density);

		return true;
	}
}
