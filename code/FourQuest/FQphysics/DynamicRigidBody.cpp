#include "DynamicRigidBody.h"
#include "EngineDataConverter.h"

#include <memory>

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
	bool DynamicRigidBody::Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics, std::shared_ptr<CollisionData> data, bool isKinematic)
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
		shape->setContactOffset(0.02f);
		shape->setRestOffset(0.01f);

		DirectX::SimpleMath::Matrix dxTransform = colliderInfo.collisionTransform.worldMatrix;
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;
		dxTransform.Decompose(scale, rotation, position);

		physx::PxTransform transform;
		CopyDirectXMatrixToPxTransform(colliderInfo.collisionTransform.worldMatrix, transform);
		mRigidDynamic = physics->createRigidDynamic(transform);
		mRigidDynamic->userData = data.get();

		if (!mRigidDynamic->attachShape(*shape))
			return false;
		physx::PxRigidBodyExt::updateMassAndInertia(*mRigidDynamic, 1.f);

		if (isKinematic)
		{
			mRigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
			mRigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, false);
		}
		else
		{
			mRigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);
			mRigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
		}

		return true;
	}

	void DynamicRigidBody::SetConvertScale(const DirectX::SimpleMath::Vector3& scale, physx::PxPhysics* physics, int* collisionMatrix)
	{
		if (fabs(mScale.x - scale.x) < 0.001f && fabs(mScale.y - scale.y) < 0.001f && fabs(mScale.z - scale.z) < 0.001f)
			return;

		mScale = scale;

		physx::PxShape* shape;
		physx::PxMaterial* material;
		mRigidDynamic->getShapes(&shape, 1);
		shape->getMaterials(&material, 1);

		if (shape->getGeometry().getType() == physx::PxGeometryType::eBOX)
		{
			physx::PxBoxGeometry boxGeometry = static_cast<const physx::PxBoxGeometry&>(shape->getGeometry());
			boxGeometry.halfExtents.x = mExtent.x * scale.x;
			boxGeometry.halfExtents.y = mExtent.y * scale.y;
			boxGeometry.halfExtents.z = mExtent.z * scale.z;
			mRigidDynamic->detachShape(*shape);
			updateShapeGeometry(mRigidDynamic, boxGeometry, physics, material, collisionMatrix);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eSPHERE)
		{
			physx::PxSphereGeometry sphereGeometry = static_cast<const physx::PxSphereGeometry&>(shape->getGeometry());
			float maxValue = std::max<float>(scale.x, std::max<float>(scale.y, scale.z));

			sphereGeometry.radius = mRadius * maxValue;
			mRigidDynamic->detachShape(*shape);
			updateShapeGeometry(mRigidDynamic, sphereGeometry, physics, material, collisionMatrix);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCAPSULE)
		{
			physx::PxCapsuleGeometry capsuleGeometry = static_cast<const physx::PxCapsuleGeometry&>(shape->getGeometry());
			float maxValue = std::max<float>(scale.y, scale.z);

			capsuleGeometry.radius = mRadius * maxValue;
			capsuleGeometry.halfHeight = mHalfHeight * scale.x;
			mRigidDynamic->detachShape(*shape);
			updateShapeGeometry(mRigidDynamic, capsuleGeometry, physics, material, collisionMatrix);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
		{
			physx::PxConvexMeshGeometry convexmeshGeometry = static_cast<const physx::PxConvexMeshGeometry&>(shape->getGeometry());
			convexmeshGeometry.scale.scale.x = scale.x;
			convexmeshGeometry.scale.scale.y = scale.y;
			convexmeshGeometry.scale.scale.z = scale.z;
			mRigidDynamic->detachShape(*shape);
			updateShapeGeometry(mRigidDynamic, convexmeshGeometry, physics, material, collisionMatrix);
		}

		shape->release();
	}
}
