#include "StaticRigidBody.h"
#include "EngineDataConverter.h"

namespace fq::physics
{
	StaticRigidBody::StaticRigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber)
		: RigidBody(colliderType, id, layerNumber)
		, mRigidStatic(nullptr)
	{
	}
	StaticRigidBody::~StaticRigidBody()
	{
	}

	bool StaticRigidBody::Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics)
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
		data->id = GetID();
		data->layerNumber = GetLayerNumber();
		shape->userData = data;

		physx::PxMat44 matrix;
		memcpy(&matrix, &colliderInfo.collisionTransform.worldMatrix, sizeof(physx::PxMat44));
		physx::PxTransform transform(matrix);

		physx::PxRigidStatic* body = physics->createRigidStatic(transform);
		if (!body->attachShape(*shape)) 
			return false;

		return true;
	}
}