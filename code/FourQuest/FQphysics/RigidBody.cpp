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
		// ���ο� shape ����
		physx::PxShape* newShape = physics->createShape(newGeometry, *material);
		physx::PxFilterData filterdata;
		filterdata.word0 = mLayerNumber;
		filterdata.word1 = collisionMatrix[mLayerNumber];
		newShape->setSimulationFilterData(filterdata);

		// ���ο� shape�� actor�� �߰�
		actor->attachShape(*newShape);

		// ���ο� shape ����
		newShape->release();
	}
}
