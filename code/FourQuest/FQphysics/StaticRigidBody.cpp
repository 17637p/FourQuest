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
		if (mRigidStatic->userData != nullptr)
		{
			delete mRigidStatic->userData;
			mRigidStatic->userData = nullptr;
		}
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
		data->myId = GetID(); 
		data->myLayerNumber = GetLayerNumber();
		shape->userData = data;
		shape->setContactOffset(0.01f);

		physx::PxTransform transform;
		CopyDirectXMatrixToPxTransform(colliderInfo.collisionTransform.worldMatrix, transform);

		mRigidStatic = physics->createRigidStatic(transform);
		mRigidStatic->userData = data;

		if (mRigidStatic == nullptr)
			return false;
		if (!mRigidStatic->attachShape(*shape))
			return false;

		return true;
	}
}
