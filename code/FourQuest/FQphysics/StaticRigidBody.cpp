#include "StaticRigidBody.h"
#include "EngineDataConverter.h"
#include "PhysicsCollisionDataManager.h"

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

		physx::PxShape* shape;
		mRigidStatic->getShapes(&shape, 1);
		mRigidStatic->detachShape(*shape);
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

		data->myId = mID;
		data->myLayerNumber = mLayerNumber;
		shape->userData = data.get();
		shape->setContactOffset(0.002f);
		shape->setRestOffset(0.001f);

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
		{
			assert(shape->getReferenceCount() == 1);
			return false;
		}

		assert(shape->getReferenceCount() == 2);

		return true;
	}

	void StaticRigidBody::SetConvertScale(const DirectX::SimpleMath::Vector3& scale, physx::PxPhysics* physics, int* collisionMatrix)
	{
		if (std::isnan(mScale.x) || std::isnan(mScale.y) || std::isnan(mScale.z))
		{
			return;
		}

		if (fabs(mScale.x - scale.x) < 0.001f && fabs(mScale.y - scale.y) < 0.001f && fabs(mScale.z - scale.z) < 0.001f)
		{
			return;
		}

		mScale = scale;

		physx::PxShape* shape;
		physx::PxMaterial* material;
		mRigidStatic->getShapes(&shape, 1);
		shape->getMaterials(&material, 1);
		void* userData = shape->userData;

		if (shape->getGeometry().getType() == physx::PxGeometryType::eBOX)
		{
			physx::PxBoxGeometry boxGeometry = static_cast<const physx::PxBoxGeometry&>(shape->getGeometry());
			boxGeometry.halfExtents.x = mExtent.x * scale.x;
			boxGeometry.halfExtents.y = mExtent.y * scale.y;
			boxGeometry.halfExtents.z = mExtent.z * scale.z;
			assert(shape->getReferenceCount() == 1);
			mRigidStatic->detachShape(*shape);
			updateShapeGeometry(mRigidStatic, boxGeometry, physics, material, collisionMatrix, userData);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eSPHERE)
		{
			physx::PxSphereGeometry sphereGeometry = static_cast<const physx::PxSphereGeometry&>(shape->getGeometry());
			float maxValue = std::max<float>(scale.x, std::max<float>(scale.y, scale.z));

			sphereGeometry.radius = mRadius * maxValue;
			assert(shape->getReferenceCount() == 1);
			mRigidStatic->detachShape(*shape);
			updateShapeGeometry(mRigidStatic, sphereGeometry, physics, material, collisionMatrix, userData);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCAPSULE)
		{
			physx::PxCapsuleGeometry capsuleGeometry = static_cast<const physx::PxCapsuleGeometry&>(shape->getGeometry());
			float maxValue = std::max<float>(scale.y, scale.z);

			capsuleGeometry.radius = mRadius * maxValue;
			capsuleGeometry.halfHeight = mHalfHeight * scale.x;
			assert(shape->getReferenceCount() == 1);
			mRigidStatic->detachShape(*shape);
			updateShapeGeometry(mRigidStatic, capsuleGeometry, physics, material, collisionMatrix, userData);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
		{
			physx::PxConvexMeshGeometry convexmeshGeometry = static_cast<const physx::PxConvexMeshGeometry&>(shape->getGeometry());
			convexmeshGeometry.scale.scale.x = scale.x;
			convexmeshGeometry.scale.scale.y = scale.y;
			convexmeshGeometry.scale.scale.z = scale.z;
			assert(shape->getReferenceCount() == 1);
			mRigidStatic->detachShape(*shape);
			updateShapeGeometry(mRigidStatic, convexmeshGeometry, physics, material, collisionMatrix, userData);
		}
	}

	bool StaticRigidBody::ChangeLayerNumber(const unsigned int& newLayerNumber, int* collisionMatrix, std::weak_ptr<PhysicsCollisionDataManager> collisionDataManager)
	{
		if (newLayerNumber == UINT_MAX)
		{
			return false;
		}

		mLayerNumber = newLayerNumber;

		physx::PxShape* shape;
		mRigidStatic->getShapes(&shape, 1);

		physx::PxFilterData newFilterData;
		newFilterData.word0 = newLayerNumber;
		newFilterData.word1 = collisionMatrix[newLayerNumber];
		shape->setSimulationFilterData(newFilterData);

		auto userData = static_cast<CollisionData*>(shape->userData);
		userData->myLayerNumber = mLayerNumber;

		return true;
	}
}
