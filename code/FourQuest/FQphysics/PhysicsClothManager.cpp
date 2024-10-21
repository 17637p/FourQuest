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

	bool PhysicsClothManager::Initialize(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager)
	{
		mPhysics = physics;
		mScene = scene;
		mCudaContextManager = cudaContextManager;

		return true;
	}

	bool PhysicsClothManager::Update()
	{
		for (auto cloth : mPhysicsClothContainer)
		{
			if (!cloth.second->UpdatePhysicsCloth(mCudaContextManager)) return false;
		}

		return true;
	}

	bool PhysicsClothManager::CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix)
	{
		std::shared_ptr<CudaPhysicsCloth> cloth = std::make_shared<CudaPhysicsCloth>(info.id, info.layerNumber);
		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();

		if (!cloth->Initialize(info, mPhysics, mScene, mCudaContextManager, collisionData, collisionMatrix))
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
			removeActorList.push_back(clothIter->second->GetPBDParticleSystem());
			mPhysicsClothContainer.erase(clothIter);

			return true;
		}

		return false;
	}
}