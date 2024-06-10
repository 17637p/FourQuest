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
		, mScale(DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f))
	{
	}

	RigidBody::~RigidBody()
	{
	}

	void RigidBody::updateShapeGeometry(physx::PxRigidActor* actor, const physx::PxGeometry& newGeometry, physx::PxPhysics* physics, physx::PxMaterial* material, int* collisionMatrix)
	{
		// 货肺款 shape 积己
		physx::PxShape* newShape = physics->createShape(newGeometry, *material);
		physx::PxFilterData filterdata;
		filterdata.word0 = mLayerNumber;
		filterdata.word1 = collisionMatrix[mLayerNumber];
		newShape->setSimulationFilterData(filterdata);
		newShape->setContactOffset(0.02f);
		newShape->setRestOffset(0.01f);

		if (mColliderType == EColliderType::COLLISION)
		{
			newShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		}
		else
		{
			newShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			newShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
		}

		// 货肺款 shape甫 actor俊 眠啊
		actor->attachShape(*newShape);

		// 货肺款 shape 秦力
		newShape->release();
	}
}
