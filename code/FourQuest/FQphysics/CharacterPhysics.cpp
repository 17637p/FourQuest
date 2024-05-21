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
	}

	bool CharacterPhysics::Initialize(const CharacterPhysicsInfo& info, physx::PxPhysics* physics)
	{
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

	bool CharacterPhysics::AddArticulationLink(const CharacterLinkInfo& info, const DirectX::SimpleMath::Vector3& extent, std::shared_ptr<CollisionData> collisionData)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		link->Initialize(info, mLinkContainer.find(info.parentBoneName)->second, mPxArticulation);
		link->CreateShape(mMaterial, extent, collisionData);

		return true;
	}
	bool CharacterPhysics::AddArticulationLink(const CharacterLinkInfo& info, const float& radius, std::shared_ptr<CollisionData> collisionData)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		link->Initialize(info, mLinkContainer.find(info.parentBoneName)->second, mPxArticulation);
		link->CreateShape(mMaterial, radius, collisionData);

		return true;
	}
	bool CharacterPhysics::AddArticulationLink(const CharacterLinkInfo& info, const float& halfHeight, const float& radius, std::shared_ptr<CollisionData> collisionData)
	{
		std::shared_ptr<CharacterLink> link = std::make_shared<CharacterLink>();

		link->Initialize(info, mLinkContainer.find(info.parentBoneName)->second, mPxArticulation);
		link->CreateShape(mMaterial, halfHeight, radius, collisionData);

		return true;
	}
}

