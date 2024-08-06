#include "CharacterLink.h"

#include "EngineDataConverter.h"
#include "CharacterJoint.h"

namespace fq::physics
{
	CharacterLink::CharacterLink()
		: mName()
		, mDensity()
		, mLocalTransform(DirectX::SimpleMath::Matrix::Identity)
		, mWorldTransform(DirectX::SimpleMath::Matrix::Identity)
		, mMyJoint(std::make_shared<CharacterJoint>())
		, mParentLink()
		, mMyChildrenLink()
		, mPxLink(nullptr)
	{
	}

	CharacterLink::~CharacterLink()
	{

	}

	bool CharacterLink::Initialize(const LinkInfo& info, std::shared_ptr<CharacterLink> parentLink, physx::PxArticulationReducedCoordinate* pxArticulation, physx::PxScene* scene)
	{
		mName = info.boneName;
		mDensity = info.density;
		mLocalTransform = info.localTransform;
		mParentLink = parentLink;
		mScene = scene;

		physx::PxTransform pxLocalTransform;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 position;
		mLocalTransform.Decompose(scale, rotation, position);

		DirectX::SimpleMath::Matrix dxTransform =
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation)
			* DirectX::SimpleMath::Matrix::CreateTranslation(position);
		
		CopyDirectXMatrixToPxTransform(mLocalTransform, pxLocalTransform);

		if (parentLink == nullptr)
		{
			mPxLink = pxArticulation->createLink(nullptr, pxLocalTransform);
		}
		else
		{
			//DirectX::SimpleMath::Matrix boneTransform = info.boneTransform;
			//DirectX::SimpleMath::Matrix jointTransform = info.jointInfo.localTransform;
			//DirectX::SimpleMath::Matrix rootTransform = info.rootTransform;
			//DirectX::SimpleMath::Matrix parentTransform = parentLink->GetWorldTransform();

			//DirectX::SimpleMath::Vector3 boneScale;
			//DirectX::SimpleMath::Quaternion boneRotation;
			//DirectX::SimpleMath::Vector3 bonePosition;
			//boneTransform.Decompose(boneScale, boneRotation, bonePosition);
			//DirectX::SimpleMath::Vector3 jointScale;
			//DirectX::SimpleMath::Quaternion jointRotation;
			//DirectX::SimpleMath::Vector3 jointPosition;
			//jointTransform.Decompose(jointScale, jointRotation, jointPosition);

			//mWorldTransform =
			//	DirectX::SimpleMath::Matrix::CreateFromQuaternion(boneRotation)
			//	//* DirectX::SimpleMath::Matrix::CreateFromQuaternion(jointRotation).Invert()
			//	* DirectX::SimpleMath::Matrix::CreateTranslation(bonePosition);
			//	//* DirectX::SimpleMath::Matrix::CreateTranslation(jointPosition).Invert();

			//DirectX::SimpleMath::Matrix dxTransform =
			//	DirectX::SimpleMath::Matrix::CreateFromQuaternion(boneRotation)
			//	//* DirectX::SimpleMath::Matrix::CreateFromQuaternion(jointRotation).Invert()
			//	* DirectX::SimpleMath::Matrix::CreateTranslation(bonePosition)
			//	//* DirectX::SimpleMath::Matrix::CreateTranslation(jointPosition).Invert()
			//	* parentLink->GetWorldTransform().Invert();

			//mLocalTransform = dxTransform;

			mPxLink = pxArticulation->createLink(parentLink->GetPxLink(), pxLocalTransform);
			mMyJoint->Initialize(mParentLink.lock(), shared_from_this(), info.jointInfo);
		}

		mPxLink->setMaxAngularVelocity(4.f);
		mPxLink->setMaxLinearVelocity(4.f);
		mPxLink->setAngularDamping(0.2f);
		mPxLink->setLinearDamping(0.2f);
		mPxLink->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

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

	void CharacterLink::SetWorldTransform(const DirectX::SimpleMath::Matrix& boneWorldTransform)
	{
		DirectX::SimpleMath::Matrix objectTransform = boneWorldTransform;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 position;
		objectTransform.Decompose(scale, rotation, position);

		DirectX::SimpleMath::Matrix dxTransform = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation) 
			* DirectX::SimpleMath::Matrix::CreateTranslation(position)
			* mMyJoint->GetLocalTransform().Invert();

		physx::PxTransform pxTransform;
		CopyDirectXMatrixToPxTransform(dxTransform, pxTransform);

		mPxLink->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);

		physx::PxTransform prevTransform = mPxLink->getGlobalPose();
		mPxLink->setGlobalPose(pxTransform);
	}
}
