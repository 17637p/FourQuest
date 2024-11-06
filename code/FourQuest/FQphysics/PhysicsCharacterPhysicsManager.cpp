#include "PhysicsCharacterPhysicsManager.h"

#include "CharacterPhysics.h"
#include "EngineDataConverter.h"
#include "CharacterLink.h"
#include "CharacterJoint.h"

#include "spdlog/spdlog.h"

namespace fq::physics
{
	PhysicsCharacterPhysicsManager::PhysicsCharacterPhysicsManager()
		: mPhysics(nullptr)
		, mScene(nullptr)
		, mCharacterPhysicsContainer()
		, mCollisionDataManager()
		, mSimulationCharacterPhysicsContainer{}
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

	bool PhysicsCharacterPhysicsManager::Update()
	{
		for (auto& [name, link] : mCharacterPhysicsContainer)
		{
			if (!link->Update()) return false;
		}

		return true;
	}

	bool PhysicsCharacterPhysicsManager::CreateCharacterphysics(const ArticulationInfo& info)
	{
		if (mCharacterPhysicsContainer.find(info.id) != mCharacterPhysicsContainer.end())
		{
			spdlog::warn("[PhysicsCharacterPhysicsManager ({})] Already Create CharacterPhysics ID : {}", __LINE__, info.id);
			return false;
		}

		std::shared_ptr<CharacterPhysics> characterPhysics = std::make_shared<CharacterPhysics>();

		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();
		collisionData->myId = info.id;
		collisionData->myLayerNumber = info.layerNumber;
		mCollisionDataManager.lock()->Create(info.id, collisionData);

		characterPhysics->Initialize(info, mPhysics, collisionData, mScene);
		mCharacterPhysicsContainer.insert(std::make_pair(info.id, characterPhysics));

		return true;
	}

	bool PhysicsCharacterPhysicsManager::RemoveArticulation(const unsigned int& id)
	{
		if (mCharacterPhysicsContainer.find(id) == mCharacterPhysicsContainer.end())
		{
			spdlog::warn("[PhysicsCharacterPhysicsManager ({})] Failed Remove Articulation. Articulation Container Have not Data ID : {}", __LINE__, id);
			return false;
		}

		auto articulationIter = mCharacterPhysicsContainer.find(id);
		auto pxArticulation = articulationIter->second->GetPxArticulation();
		
		if (articulationIter->second->GetIsRagdoll() == true)
		{ 
			// 모든 링크가 제거된 후 Articulation을 Scene에서 제거합니다.
			mScene->removeArticulation(*pxArticulation);
			PX_RELEASE(pxArticulation);
		}

		// 컨테이너에서 해당 Articulation을 삭제합니다.
		mCharacterPhysicsContainer.erase(articulationIter);

		return true;
	}

	bool PhysicsCharacterPhysicsManager::RemoveAllArticulation()
	{
		// 모든 관절 삭제
		for (auto [id, articulation] : mCharacterPhysicsContainer)
		{
			auto pxArticulation = articulation->GetPxArticulation();

			if (articulation->GetIsRagdoll() == true)
			{
				// 모든 링크가 제거된 후 Articulation을 Scene에서 제거합니다.
				mScene->removeArticulation(*pxArticulation);
				PX_RELEASE(pxArticulation);

				spdlog::trace("[PxScene Remove] PxScene's Articulation Current Count : {}", mScene->getNbArticulations());
			}
		}

		mCharacterPhysicsContainer.clear();

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
		
		for (auto& [name, link] : articulation->GetLinkContainer())
		{
			fq::physics::ArticulationLinkGetData data;

			data.jointLocalTransform = link->GetCharacterJoint()->GetSimulationTransform();
			data.name = link->GetName();

			articulationData.linkData.push_back(data);
		}
	}

	void PhysicsCharacterPhysicsManager::SetArticulationData(const unsigned int& id, const ArticulationSetData& articulationData, int* collisionMatrix)
	{
		auto articulationIter = mCharacterPhysicsContainer.find(id);
		auto articulation = articulationIter->second;

		articulation->ChangeLayerNumber(articulationData.myLayerNumber, collisionMatrix, mCollisionDataManager.lock());

		if (articulationData.bIsRagdollSimulation != articulation->GetIsRagdoll())
		{
			articulation->SetIsRagdoll(articulationData.bIsRagdollSimulation);
			articulation->SetWorldTransform(articulationData.worldTransform);

			if (articulationData.bIsRagdollSimulation)
			{
				for (const auto& linkData : articulationData.linkData)
				{
					articulation->SetLinkTransformUpdate(linkData.name, linkData.boneWorldTransform);
				}
				auto pxArticulation = articulation->GetPxArticulation();
				bool isCheck = mScene->addArticulation(*pxArticulation);

				spdlog::trace("[Simulation] Articulation ID : {}, Count : {}", id, mScene->getNbArticulations());
				assert(isCheck);
			}
		}
	}
}
