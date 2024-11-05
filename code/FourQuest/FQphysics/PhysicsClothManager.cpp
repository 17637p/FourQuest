#include "PhysicsClothManager.h"

#include "CudaPhysicsCloth.h"
#include "FQCommonPhysics.h"
#include "PhysicsCollisionDataManager.h"

#include <spdlog\spdlog.h>

namespace fq::physics
{
	PhysicsClothManager::PhysicsClothManager()
		: mCudaContextManager(nullptr)
		, mScene(nullptr)
		, mPhysics(nullptr)
		, mPhysicsClothContainer()
	{
	}

	PhysicsClothManager::~PhysicsClothManager()
	{
	}

	bool PhysicsClothManager::Initialize(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager)
	{
		mPhysics = physics;
		mScene = scene;
		mCudaContextManager = cudaContextManager;
		mCollisionDataManager = collisionDataManager;

		return true;
	}

	bool PhysicsClothManager::Update(float deltaTime)
	{
		for (auto cloth : mPhysicsClothContainer)
		{
			if (!cloth.second->UpdatePhysicsCloth(mCudaContextManager)) return false;
		}

		for (auto [clothData, collisionMatrix] : mUpCommingClothVec)
		{
			CreateCloth(clothData, collisionMatrix);
		}
		mUpCommingClothVec.clear();

		return true;
	}

	bool PhysicsClothManager::UpdateLerpSimulationPositions()
	{



		return true;
	}

	bool PhysicsClothManager::CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix, bool isSkinnedMesh)
	{
		mUpCommingClothVec.push_back(std::make_pair(info, collisionMatrix));

 		return true;
	}

	bool PhysicsClothManager::CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix)
	{
		std::shared_ptr<CudaPhysicsCloth> cloth = std::make_shared<CudaPhysicsCloth>(info.id, info.layerNumber);
		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();

		if (!cloth->Initialize(info, mPhysics, mScene, mCudaContextManager, collisionData, collisionMatrix, false))
		{
			spdlog::error("[PhysicsClothManager ({})] Failed Create Cloth", __LINE__);
			return false;
		}

		mPhysicsClothContainer.insert(std::make_pair(info.id, cloth));
		mCollisionDataManager.lock()->Create(info.id, collisionData);

		return true;
	}

	bool PhysicsClothManager::GetClothData(unsigned int id, Cloth::GetSetClothData& data)
	{
		auto clothIter = mPhysicsClothContainer.find(id);

		if (clothIter != mPhysicsClothContainer.end())
		{
			clothIter->second->GetPhysicsCloth(data);
			return true;
		}

		return false;
	}

	bool PhysicsClothManager::SetClothData(unsigned int id, const Cloth::GetSetClothData& data)
	{
		auto clothIter = mPhysicsClothContainer.find(id);

		if (clothIter != mPhysicsClothContainer.end())
		{
			clothIter->second->SetPhysicsCloth(data);
			return true;
		}

		return false;
	}
	bool PhysicsClothManager::RemoveCloth(unsigned int id, std::vector<physx::PxActor*>& removeActorList)
	{
		auto clothIter = mPhysicsClothContainer.find(id);

		if (clothIter != mPhysicsClothContainer.end())
		{
			mScene->removeActor(*clothIter->second->GetPBDParticleSystem());
			mPhysicsClothContainer.erase(clothIter);

			return true;
		}

		return false;
	}
	bool PhysicsClothManager::RemoveAllCloth(std::vector<physx::PxActor*>& removeActorList)
	{
		for (auto [id, actor] : mPhysicsClothContainer)
		{
			mScene->removeActor(*actor->GetPBDParticleSystem());
		}
		mPhysicsClothContainer.clear();
		mUpCommingClothVec.clear();

		return true;
	}

	const std::vector<DirectX::SimpleMath::Vector3>& PhysicsClothManager::GetClothVertex(unsigned int id)
	{
		auto clothIter = mPhysicsClothContainer.find(id);

		if (clothIter != mPhysicsClothContainer.end())
		{
			auto cloth = clothIter->second;
			return cloth->GetVertices();
		}

		return std::vector<DirectX::SimpleMath::Vector3>();
	}

	const std::vector<unsigned int>& PhysicsClothManager::GetClothIndices(unsigned int id)
	{
		auto clothIter = mPhysicsClothContainer.find(id);

		if (clothIter != mPhysicsClothContainer.end())
		{
			auto cloth = clothIter->second;
			return cloth->GetIndices();
		}

		return std::vector<unsigned int>();
	}
}