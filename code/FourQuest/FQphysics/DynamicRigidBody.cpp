#include "DynamicRigidBody.h"
#include "EngineDataConverter.h"

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
	bool DynamicRigidBody::Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics, std::shared_ptr<CollisionData> data)
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

		data->myId = GetID();
		data->myLayerNumber = GetLayerNumber();
		shape->userData = data.get();
		shape->setContactOffset(0.01f);

		physx::PxTransform transform;
		CopyDirectXMatrixToPxTransform(colliderInfo.collisionTransform.worldMatrix, transform);

		mRigidDynamic = physics->createRigidDynamic(transform);
		mRigidDynamic->userData = data.get();

		if (!mRigidDynamic->attachShape(*shape))
			return false;
		physx::PxRigidBodyExt::updateMassAndInertia(*mRigidDynamic, 1.f);

		return true;
	}
}
