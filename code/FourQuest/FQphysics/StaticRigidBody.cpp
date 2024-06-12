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
		CollisionData* data = (CollisionData*)mRigidStatic->userData;
		data->isDead = true;
	}

	bool StaticRigidBody::Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics, std::shared_ptr<CollisionData> data)
	{
		if (mColliderType == EColliderType::COLLISION)
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
		shape->setContactOffset(0.02f);
		shape->setRestOffset(0.01f);

		DirectX::SimpleMath::Matrix dxTransform = colliderInfo.collisionTransform.worldMatrix;
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;
		dxTransform.Decompose(scale, rotation, position);

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

	void StaticRigidBody::SetConvertScale(const DirectX::SimpleMath::Vector3& scale, physx::PxPhysics* physics, int* collisionMatrix)
	{
		if (std::isnan(mScale.x) || std::isnan(mScale.y) || std::isnan(mScale.z))
			return;

		if (fabs(mScale.x - scale.x) < 0.001f && fabs(mScale.y - scale.y) < 0.001f && fabs(mScale.z - scale.z) < 0.001f)
			return;

		mScale = scale;

		physx::PxShape* shape;
		physx::PxMaterial* material;
		mRigidStatic->getShapes(&shape, 1);
		shape->getMaterials(&material, 1);

		if (shape->getGeometry().getType() == physx::PxGeometryType::eBOX)
		{
			physx::PxBoxGeometry boxGeometry = static_cast<const physx::PxBoxGeometry&>(shape->getGeometry());
			boxGeometry.halfExtents.x = mExtent.x * scale.x;
			boxGeometry.halfExtents.y = mExtent.y * scale.y;
			boxGeometry.halfExtents.z = mExtent.z * scale.z;
			mRigidStatic->detachShape(*shape);
			updateShapeGeometry(mRigidStatic, boxGeometry, physics, material, collisionMatrix);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eSPHERE)
		{
			physx::PxSphereGeometry sphereGeometry = static_cast<const physx::PxSphereGeometry&>(shape->getGeometry());
			float maxValue = std::max<float>(scale.x, std::max<float>(scale.y, scale.z));

			sphereGeometry.radius = mRadius * maxValue;
			mRigidStatic->detachShape(*shape);
			updateShapeGeometry(mRigidStatic, sphereGeometry, physics, material, collisionMatrix);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCAPSULE)
		{
			physx::PxCapsuleGeometry capsuleGeometry = static_cast<const physx::PxCapsuleGeometry&>(shape->getGeometry());
			float maxValue = std::max<float>(scale.y, scale.z);

			capsuleGeometry.radius = mRadius * maxValue;
			capsuleGeometry.halfHeight = mHalfHeight * scale.x;
			mRigidStatic->detachShape(*shape);
			updateShapeGeometry(mRigidStatic, capsuleGeometry, physics, material, collisionMatrix);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
		{
			physx::PxConvexMeshGeometry convexmeshGeometry = static_cast<const physx::PxConvexMeshGeometry&>(shape->getGeometry());
			convexmeshGeometry.scale.scale.x = scale.x;
			convexmeshGeometry.scale.scale.y = scale.y;
			convexmeshGeometry.scale.scale.z = scale.z;
			mRigidStatic->detachShape(*shape);
			updateShapeGeometry(mRigidStatic, convexmeshGeometry, physics, material, collisionMatrix);
		}

		shape->release();
	}
}
