#include "CharacterPhysics.h"

#include "CharacterLink.h"
#include "EngineDataConverter.h"

namespace fq::physics
{
	CharacterPhysics::CharacterPhysics()
		: mPxArticulation(nullptr)
		, mMaterial(nullptr)
		, mRootLink(std::make_shared<CharacterLink>())
		, mModelPath()
		, mID()
		, mLayerNumber()
		, mLinkContainer()
		, mWorldTransform()
	{
	}

	CharacterPhysics::~CharacterPhysics()
	{
		mCollisionData->isDead = true;
	}

	bool CharacterPhysics::Initialize(const CharacterPhysicsInfo& info, physx::PxPhysics* physics, std::shared_ptr<CollisionData> collisionData)
	{
		mCollisionData = collisionData;
		mPxArticulation = physics->createArticulationReducedCoordinate();
		mPxArticulation->setArticulationFlag(physx::PxArticulationFlag::eFIX_BASE, false);
		mPxArticulation->setArticulationFlag(physx::PxArticulationFlag::eDISABLE_SELF_COLLISION, false);
		mPxArticulation->setSolverIterationCounts(4);
		mPxArticulation->setMaxCOMAngularVelocity(10000.f);

		mMaterial = physics->createMaterial(info.staticFriction, info.dynamicFriction, info.restitution);
		mModelPath = info.modelPath;
		mID = info.id;
		mLayerNumber = info.layerNumber;
		mWorldTransform = info.worldTransform;

		physx::PxTransform pxTransform;
		CopyDirectXMatrixToPxTransform(mWorldTransform, pxTransform);

		CharacterLinkInfo linkInfo;
		std::string str = "root";
		linkInfo.boneName = "root";
		linkInfo.parentBoneName = "";
		mRootLink->Initialize(linkInfo, nullptr, mPxArticulation);

		return true;
	}

	bool CharacterPhysics::AddArticulationLink(const CharacterLinkInfo& info, int* collisionMatrix, const DirectX::SimpleMath::Vector3& extent)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		auto parentLink = mLinkContainer.find(info.parentBoneName);
		if (parentLink == mLinkContainer.end())
		{
			if (!link->Initialize(info, mRootLink, mPxArticulation)) return false;
		}
		else
		{
			if (!link->Initialize(info, parentLink->second, mPxArticulation)) return false;
		}

		mLinkContainer.insert(std::make_pair(info.boneName, link));
		physx::PxShape* shape = link->CreateShape(mMaterial, extent, mCollisionData);
		assert(shape);

		physx::PxFilterData filterdata;
		filterdata.word0 = mLayerNumber;
		filterdata.word1 = collisionMatrix[mLayerNumber];
		shape->setSimulationFilterData(filterdata);

		return true;
	}
	bool CharacterPhysics::AddArticulationLink(const CharacterLinkInfo& info, int* collisionMatrix, const float& radius)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		auto parentLink = mLinkContainer.find(info.parentBoneName);
		if (parentLink == mLinkContainer.end())
		{
			if (!link->Initialize(info, mRootLink, mPxArticulation)) return false;
		}
		else
		{
			if (!link->Initialize(info, parentLink->second, mPxArticulation)) return false;
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
	bool CharacterPhysics::AddArticulationLink(const CharacterLinkInfo& info, int* collisionMatrix, const float& halfHeight, const float& radius)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		auto parentLink = mLinkContainer.find(info.parentBoneName);
		if (parentLink == mLinkContainer.end())
		{
			if (!link->Initialize(info, mRootLink, mPxArticulation)) return false;
		}
		else
		{
			if (!link->Initialize(info, parentLink->second, mPxArticulation)) return false;
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
}

