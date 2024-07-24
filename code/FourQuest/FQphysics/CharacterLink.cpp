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
		int shapeCount = mPxLink->getNbShapes();

		physx::PxShape* shape;
		mPxLink->getShapes(&shape, shapeCount);

		for (int i = 0; i < shapeCount; i++)
		{
			mPxLink->detachShape(*shape);
			PX_ASSERT(shape);
		}
	}

	bool CharacterLink::Initialize(const LinkInfo& info, std::shared_ptr<CharacterLink> parentLink, physx::PxArticulationReducedCoordinate* pxArticulation, physx::PxScene* scene)
	{
		mName = info.boneName;
		mDensity = info.density;
		mLocalTransform = info.localTransform;
		mParentLink = parentLink;
		mScene = scene;

		physx::PxTransform pxLocalTransform;
		CopyDirectXMatrixToPxTransform(mLocalTransform, pxLocalTransform);

		if (parentLink == nullptr)
		{
			mPxLink = pxArticulation->createLink(nullptr, pxLocalTransform);
		}
		else
		{
			mPxLink = pxArticulation->createLink(parentLink->GetPxLink(), pxLocalTransform);
			mMyJoint->Initialize(mParentLink.lock(), shared_from_this(), info.jointInfo);
		}

		mPxLink->setMaxAngularVelocity(4.f);
		mPxLink->setMaxLinearVelocity(4.f);
		mPxLink->setAngularDamping(0.2f);
		mPxLink->setLinearDamping(0.2f);

		return true;
	}

	bool CharacterLink::Update()
	{
		physx::PxTransform pxTransform;
		pxTransform = mPxLink->getGlobalPose();

		CopyDirectXMatrixToPxTransform(mWorldTransform, pxTransform);

		return mMyJoint->Update(mParentLink.lock()->GetPxLink());
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
