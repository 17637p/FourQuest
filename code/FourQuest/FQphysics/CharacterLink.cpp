#include "CharacterLink.h"

#include "EngineDataConverter.h"
#include "CharacterJoint.h"

namespace fq::physics
{
	CharacterLink::CharacterLink()
		: mName()
		, mDensity()
		, mLocalTransform()
		, mMyJoint(nullptr)
		, mParentLink()
		, mMyChildrenLink()
		, mPxLink(nullptr)
	{
	}

	CharacterLink::~CharacterLink()
	{

	}

	bool CharacterLink::Initialize(const CharacterLinkInfo& info, std::shared_ptr<CharacterLink> parentLink, physx::PxArticulationReducedCoordinate* pxArticulation)
	{
		mName = info.boneName;
		mDensity = info.density;
		mLocalTransform = info.localTransform;
		mParentLink = parentLink;

		physx::PxTransform pxLocalTransform;
		CopyDirectXMatrixToPxTransform(mLocalTransform, pxLocalTransform);

		if (parentLink == nullptr)
			mPxLink = pxArticulation->createLink(nullptr, pxLocalTransform);
		else
		{
			mPxLink = pxArticulation->createLink(parentLink->GetPxLink(), pxLocalTransform);
			mMyJoint->Initialize(mParentLink.lock(), shared_from_this(), info.characterJointInfo);
		}

		return true;
	}

	bool CharacterLink::CreateShape(const physx::PxMaterial* material, const DirectX::SimpleMath::Vector3& extent, std::shared_ptr<CollisionData> collisionData)
	{
		physx::PxVec3 pxExtent;
		CopyDxVec3ToPxVec3(extent, pxExtent);

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*mPxLink, physx::PxBoxGeometry(pxExtent), *material);
		physx::PxRigidBodyExt::updateMassAndInertia(*mPxLink, mDensity);

		shape->userData = collisionData.get();

		return true;
	}
	bool CharacterLink::CreateShape(const physx::PxMaterial* material, const float& radius, const float& halfHeight, std::shared_ptr<CollisionData> collisionData)
	{
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*mPxLink, physx::PxCapsuleGeometry(radius, halfHeight), *material);
		physx::PxRigidBodyExt::updateMassAndInertia(*mPxLink, mDensity);

		shape->userData = collisionData.get();

		return true;
	}
	bool CharacterLink::CreateShape(const physx::PxMaterial* material, const float& radius, std::shared_ptr<CollisionData> collisionData)
	{
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*mPxLink, physx::PxSphereGeometry(radius), *material);
		physx::PxRigidBodyExt::updateMassAndInertia(*mPxLink, mDensity);

		shape->userData = collisionData.get();

		return true;
	}
}
