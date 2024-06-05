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

		DirectX::SimpleMath::Matrix dxTransform = colliderInfo.collisionTransform.worldMatrix;
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;
		dxTransform.Decompose(scale, rotation, position);
		mScale = scale;

		physx::PxTransform transform;
		CopyDirectXMatrixToPxTransform(colliderInfo.collisionTransform.worldMatrix, transform);

		mRigidDynamic = physics->createRigidDynamic(transform);
		mRigidDynamic->userData = data.get();

		if (!mRigidDynamic->attachShape(*shape))
			return false;
		physx::PxRigidBodyExt::updateMassAndInertia(*mRigidDynamic, 1.f);

		return true;
	}

	void updateShapeGeometry(physx::PxRigidActor* actor, const physx::PxGeometry& newGeometry, physx::PxPhysics* physics, physx::PxMaterial* material)
	{
		// 货肺款 shape 积己
		physx::PxShape* newShape = physics->createShape(newGeometry, *material);

		// 货肺款 shape甫 actor俊 眠啊
		actor->attachShape(*newShape);

		// 货肺款 shape 秦力
		newShape->release();
	}

	void DynamicRigidBody::SetConvertScale(const DirectX::SimpleMath::Vector3& scale, physx::PxPhysics* physics)
	{
		if (abs(mScale.Length()) + 0.1f >= abs(scale.Length()))
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
			updateShapeGeometry(mRigidDynamic, boxGeometry, physics, material);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eSPHERE)
		{
			physx::PxSphereGeometry sphereGeometry = static_cast<const physx::PxSphereGeometry&>(shape->getGeometry());
			float maxValue = std::max<float>(scale.x, std::max<float>(scale.y, scale.z));

			sphereGeometry.radius = mRadius * maxValue;
			mRigidDynamic->detachShape(*shape);
			updateShapeGeometry(mRigidDynamic, sphereGeometry, physics, material);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCAPSULE)
		{
			physx::PxCapsuleGeometry capsuleGeometry = static_cast<const physx::PxCapsuleGeometry&>(shape->getGeometry());
			float maxValue = std::max<float>(scale.y, scale.z);

			capsuleGeometry.radius = mRadius * maxValue;
			capsuleGeometry.halfHeight = mHalfHeight * scale.x;
			mRigidDynamic->detachShape(*shape);
			updateShapeGeometry(mRigidDynamic, capsuleGeometry, physics, material);
		}
		else if (shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
		{
			physx::PxConvexMeshGeometry convexmeshGeometry = static_cast<const physx::PxConvexMeshGeometry&>(shape->getGeometry());
			convexmeshGeometry.scale.scale.x = scale.x;
			convexmeshGeometry.scale.scale.y = scale.y;
			convexmeshGeometry.scale.scale.z = scale.z;
			mRigidDynamic->detachShape(*shape);
			updateShapeGeometry(mRigidDynamic, convexmeshGeometry, physics, material);
		}
	}
}
