#include "CharacterLink.h"

#include "EngineDataConverter.h"
#include "CharacterJoint.h"

namespace fq::physics
{
	CharacterLink::CharacterLink()
		: mName()
		, mDensity()
		, mLocalTransform()
		, mMyJoint(std::make_shared<CharacterJoint>())
		, mParentLink()
		, mMyChildrenLink()
		, mPxLink(nullptr)
	{
	}

	CharacterLink::~CharacterLink()
	{
	}

	bool CharacterLink::Initialize(const LinkInfo& info, std::shared_ptr<CharacterLink> parentLink, physx::PxArticulationReducedCoordinate* pxArticulation)
	{
		mName = info.boneName;
		mDensity = info.density;
		mLocalTransform = info.localTransform;
		mParentLink = parentLink;

		physx::PxTransform pxLocalTransform;
		CopyDirectXMatrixToPxTransformXYZ(mLocalTransform, pxLocalTransform);

		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 position;
		mLocalTransform.Decompose(scale, rotation, position);

		if (parentLink == nullptr)
		{
			mPxLink = pxArticulation->createLink(nullptr, pxLocalTransform);
		}
		else
		{
			mPxLink = pxArticulation->createLink(parentLink->GetPxLink(), pxLocalTransform);
			mMyJoint->Initialize(mParentLink.lock(), shared_from_this(), info.jointInfo);
		}

		mPxLink->setMaxAngularVelocity(1.f);
		mPxLink->setMaxLinearVelocity(2.f);
		mPxLink->setLinearDamping(2.f);
		mPxLink->setAngularDamping(2.f);
		mPxLink->setMaxDepenetrationVelocity(1.0f);
		mPxLink->setMassSpaceInertiaTensor(physx::PxVec3(0.01f, 0.01f, 0.01f));

		return true;
	}
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	physx::PxShape* CharacterLink::CreateShape(const physx::PxMaterial* material, const DirectX::SimpleMath::Vector3& extent, std::shared_ptr<CollisionData> collisionData)
	{
		physx::PxVec3 pxExtent;
		std::memcpy(&pxExtent, &extent, sizeof(pxExtent));

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*mPxLink, physx::PxBoxGeometry(pxExtent), *material);
		physx::PxRigidBodyExt::updateMassAndInertia(*mPxLink, mDensity);

		shape->userData = collisionData.get();
		shape->setContactOffset(0.002f);
		shape->setRestOffset(0.001f);
		return shape;
	}
	physx::PxShape* CharacterLink::CreateShape(const physx::PxMaterial* material, const float& radius, const float& halfHeight, std::shared_ptr<CollisionData> collisionData)
	{
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*mPxLink, physx::PxCapsuleGeometry(radius, halfHeight), *material);
		physx::PxRigidBodyExt::updateMassAndInertia(*mPxLink, mDensity);

		shape->userData = collisionData.get();
		shape->setContactOffset(0.002f);
		shape->setRestOffset(0.001f);
		return shape;
	}
	physx::PxShape* CharacterLink::CreateShape(const physx::PxMaterial* material, const float& radius, std::shared_ptr<CollisionData> collisionData)
	{
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*mPxLink, physx::PxSphereGeometry(radius), *material);
		physx::PxRigidBodyExt::updateMassAndInertia(*mPxLink, mDensity);
		
		shape->userData = collisionData.get();
		shape->setContactOffset(0.002f);
		shape->setRestOffset(0.001f);
		return shape;
	}

	bool CharacterLink::ChangeLayerNumber(const physx::PxFilterData& filterData, CollisionData* collisionData)
	{
		physx::PxShape* shape;
		mPxLink->getShapes(&shape, 1);

		shape->setSimulationFilterData(filterData);
		shape->userData = collisionData;

		return true;
	}
}
