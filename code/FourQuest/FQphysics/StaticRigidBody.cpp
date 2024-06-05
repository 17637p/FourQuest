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

	bool StaticRigidBody::Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics, std::shared_ptr<CollisionData> data)
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

		mRigidStatic = physics->createRigidStatic(transform);
		mRigidStatic->userData = data.get();

		if (mRigidStatic == nullptr)
			return false;
		if (!mRigidStatic->attachShape(*shape))
			return false;

		return true;
	}

	void StaticRigidBody::SetScale(const DirectX::SimpleMath::Vector3& scale)
	{
		physx::PxShape* shape;
		mRigidStatic->getShapes(&shape, 1);

		if (shape->getGeometry().getType() == physx::PxGeometryType::eBOX)
		{
			physx::PxBoxGeometry boxGeometry = static_cast<const physx::PxBoxGeometry&>(shape->getGeometry());
			boxGeometry.halfExtents.x = mExtent.x * scale.x;
			boxGeometry.halfExtents.y = mExtent.y * scale.y;
			boxGeometry.halfExtents.z = mExtent.z * scale.z;
			shape->setGeometry(boxGeometry);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eSPHERE)
		{
			physx::PxSphereGeometry sphereGeometry;
			float maxValue = std::max<float>(scale.x, std::max<float>(scale.y, scale.z));

			sphereGeometry.radius = mRadius * maxValue;
			shape->setGeometry(sphereGeometry);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCAPSULE)
		{
			physx::PxCapsuleGeometry capsuleGeometry;
			float maxValue = std::max<float>(scale.y, scale.z);

			capsuleGeometry.radius = mRadius * maxValue;
			capsuleGeometry.halfHeight = mHalfHeight * scale.x;
			shape->setGeometry(capsuleGeometry);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
		{
			physx::PxConvexMeshGeometry convexmeshGeometry = static_cast<const physx::PxConvexMeshGeometry&>(shape->getGeometry());
			convexmeshGeometry.scale.scale.x = scale.x;
			convexmeshGeometry.scale.scale.y = scale.y;
			convexmeshGeometry.scale.scale.z = scale.z;
			shape->setGeometry(convexmeshGeometry);
		}
	}
}
