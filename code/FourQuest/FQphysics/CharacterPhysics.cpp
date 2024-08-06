#include "CharacterPhysics.h"

#include "CharacterLink.h"
#include "EngineDataConverter.h"

#include "PhysicsCollisionDataManager.h"

namespace fq::physics
{
	CharacterPhysics::CharacterPhysics()
		: mPxArticulation(nullptr)
		, mMaterial(nullptr)
		, mRootLink(nullptr)
		, mModelPath()
		, mID()
		, mLayerNumber()
		, mLinkContainer()
		, mWorldTransform()
		, mbIsRagdoll(false)
	{
	}

	CharacterPhysics::~CharacterPhysics()
	{
		mCollisionData->isDead = true;
	}

	bool CharacterPhysics::Initialize(const ArticulationInfo& info, physx::PxPhysics* physics, std::shared_ptr<CollisionData> collisionData, physx::PxScene* scene)
	{
		mCollisionData = collisionData;
		mPxArticulation = physics->createArticulationReducedCoordinate();
		mPxArticulation->setArticulationFlag(physx::PxArticulationFlag::eFIX_BASE, false);
		mPxArticulation->setArticulationFlag(physx::PxArticulationFlag::eDISABLE_SELF_COLLISION, false);
		mPxArticulation->setSolverIterationCounts(32);
		mPxArticulation->setMaxCOMLinearVelocity(10.f);
		mPxArticulation->setMaxCOMAngularVelocity(10.f); 

		mScene = scene;
		mMaterial = physics->createMaterial(info.staticFriction, info.dynamicFriction, info.restitution);
		mID = info.id;
		mLayerNumber = info.layerNumber;
		mWorldTransform = info.worldTransform;

		return true;
	}

	bool CharacterPhysics::Update()
	{
		for (auto& [name, link] : mLinkContainer)
		{
			if (!link->Update()) return false;
		}

		return true;
	}

	bool CharacterPhysics::AddArticulationLink(const LinkInfo& info, int* collisionMatrix, const DirectX::SimpleMath::Vector3& extent)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		auto parentLink = mLinkContainer.find(info.parentBoneName);
		if (parentLink == mLinkContainer.end())
		{
			if (!link->Initialize(info, mRootLink, mPxArticulation, mScene)) return false;
		} 
		else
		{
			if (!link->Initialize(info, parentLink->second, mPxArticulation, mScene)) return false;
		}

		physx::PxShape* shape = link->CreateShape(mMaterial, extent, mCollisionData);
		assert(shape);

		physx::PxFilterData filterdata;
		filterdata.word0 = mLayerNumber;
		filterdata.word1 = collisionMatrix[mLayerNumber];
		shape->setSimulationFilterData(filterdata);

		mLinkContainer.insert(std::make_pair(info.boneName, link));

		return true;
	}
	bool CharacterPhysics::AddArticulationLink(const LinkInfo& info, int* collisionMatrix, const float& radius)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		auto parentLink = mLinkContainer.find(info.parentBoneName);
		if (parentLink == mLinkContainer.end())
		{
			if (!link->Initialize(info, mRootLink, mPxArticulation, mScene)) return false;
		}
		else
		{
			if (!link->Initialize(info, parentLink->second, mPxArticulation, mScene)) return false;
		}

		mLinkContainer.insert(std::make_pair(info.boneName, link));
		physx::PxShape* shape = link->CreateShape(mMaterial, radius, mCollisionData);
		assert(shape);

		physx::PxFilterData filterdata;
		filterdata.word0 = mLayerNumber;
		filterdata.word1 = collisionMatrix[mLayerNumber];
		shape->setSimulationFilterData(filterdata);

		return true;
	}
	bool CharacterPhysics::AddArticulationLink(const LinkInfo& info, int* collisionMatrix, const float& halfHeight, const float& radius)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		auto parentLink = mLinkContainer.find(info.parentBoneName);
		if (parentLink == mLinkContainer.end())
		{
			if (!link->Initialize(info, mRootLink, mPxArticulation, mScene)) return false;
		}
		else
		{
			if (!link->Initialize(info, parentLink->second, mPxArticulation, mScene)) return false;
		}

		mLinkContainer.insert(std::make_pair(info.boneName, link));
		physx::PxShape* shape = link->CreateShape(mMaterial, radius, halfHeight, mCollisionData);
		assert(shape);

		physx::PxFilterData filterdata;
		filterdata.word0 = mLayerNumber;
		filterdata.word1 = collisionMatrix[mLayerNumber];
		shape->setSimulationFilterData(filterdata);

		return true;
	}
	bool CharacterPhysics::AddArticulationLink(LinkInfo& info, int* collisionMatrix)
	{
		mRootLink = std::make_shared<CharacterLink>();

		info.localTransform = mWorldTransform * info.localTransform;
		mRootLink->Initialize(info, nullptr, mPxArticulation, mScene);

		return true;
	}

	bool CharacterPhysics::ChangeLayerNumber(const int& newLayerNumber, int* collisionMatrix, std::shared_ptr<PhysicsCollisionDataManager> mPhysicsCollisionDataManager)
	{
		if (newLayerNumber == UINT_MAX)
		{
			return false;
		}

		physx::PxFilterData newFilterData;
		newFilterData.word0 = newLayerNumber;
		newFilterData.word1 = collisionMatrix[newLayerNumber];

		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();
		collisionData->myId = mID;
		collisionData->myLayerNumber = newLayerNumber;

		for (const auto& [name, myLink] : mLinkContainer)
		{
			myLink->ChangeLayerNumber(newFilterData, collisionData.get());
		}
		
		mPhysicsCollisionDataManager->Create(mID, collisionData);
	}
}