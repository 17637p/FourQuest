#include "PhysicsCharacterPhysicsManager.h"

#include "CharacterPhysics.h"
#include "EngineDataConverter.h"

namespace fq::physics
{
	PhysicsCharacterPhysicsManager::PhysicsCharacterPhysicsManager()
		: mPhysics(nullptr)
		, mScene(nullptr)
		, mCharacterPhysicsContainer()
		, mCollisionDataManager()
	{
	}

	PhysicsCharacterPhysicsManager::~PhysicsCharacterPhysicsManager()
	{
	}

	bool PhysicsCharacterPhysicsManager::initialize(physx::PxPhysics* physics, physx::PxScene* scene, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager)
	{
		mCollisionDataManager = collisionDataManager;
		mPhysics = physics;
		mScene = scene;

		return true;
	}

	bool PhysicsCharacterPhysicsManager::CreateCharacterphysics(const ArticulationInfo& info)
	{
		if (mCharacterPhysicsContainer.find(info.id) != mCharacterPhysicsContainer.end())
			return false;

		std::shared_ptr<CharacterPhysics> characterPhysics = std::make_shared<CharacterPhysics>();

		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();
		collisionData->myId = info.id;
		collisionData->myLayerNumber = info.layerNumber;
		mCollisionDataManager.lock()->Create(info.id, collisionData);

		characterPhysics->Initialize(info, mPhysics, collisionData);
		mCharacterPhysicsContainer.insert(std::make_pair(info.id, characterPhysics));

		return true;
	}

	bool PhysicsCharacterPhysicsManager::RemoveArticulation(const unsigned int& id)
	{
		if (mCharacterPhysicsContainer.find(id) == mCharacterPhysicsContainer.end())
		{
			return false;
		}

		auto articulationIter = mCharacterPhysicsContainer.find(id);
		auto pxArticulation = articulationIter->second->GetPxArticulation();
		mCharacterPhysicsContainer.erase(articulationIter);

		mScene->removeArticulation(*pxArticulation);

		return true;
	}

	bool PhysicsCharacterPhysicsManager::ChangeScene()
	{


		return true;
	}
	
	void PhysicsCharacterPhysicsManager::GetArticulationData(const unsigned int& id, ArticulationGetData& articulationData)
	{
		auto articulationIter = mCharacterPhysicsContainer.find(id);
		auto articulation = articulationIter->second;

		physx::PxTransform pxTransform = articulation->GetPxArticulation()->getRootGlobalPose();
		DirectX::SimpleMath::Matrix dxTransform; 
		CopyPxTransformToDirectXMatrix(pxTransform, dxTransform);

		articulationData.worldTransform = dxTransform;
		articulationData.bIsRagdollSimulation = articulation->GetIsRagdoll();
	}

	void PhysicsCharacterPhysicsManager::SetArticulationData(const unsigned int& id, const ArticulationSetData& articulationData, int* collisionMatrix)
	{
		auto articulationIter = mCharacterPhysicsContainer.find(id);
		auto articulation = articulationIter->second;

		articulation->ChangeLayerNumber(articulationData.myLayerNumber, collisionMatrix, mCollisionDataManager.lock());

		if (articulationData.bIsRagdollSimulation != articulation->GetIsRagdoll())
		{
			articulation->SetIsRagdoll(articulationData.bIsRagdollSimulation);

			if (articulationData.bIsRagdollSimulation)
			{
				bool isCheck = mScene->addArticulation(*articulation->GetPxArticulation());
				assert(isCheck);
			}
			else
			{
				mScene->removeArticulation(*articulation->GetPxArticulation());
			}
		}
	}
}
