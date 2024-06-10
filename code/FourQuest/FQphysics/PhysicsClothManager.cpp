#include "PhysicsClothManager.h"

#include "PhysicsCloth.h"

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

	bool PhysicsClothManager::Initialize(physx::PxFoundation* foundation, physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager)
	{
		mPhysics = physics;
		mScene = scene;
		mCudaContextManager = cudaContextManager;

		return true;
	}
	bool PhysicsClothManager::CreateCloth(const PhysicsClothInfo& info)
	{
		std::shared_ptr<PhysicsCloth> cloth = std::make_shared<PhysicsCloth>(info.id, info.layerNumber);

		if (!cloth->Initialize(info, mPhysics, mScene, mCudaContextManager))
			return false;

		mPhysicsClothContainer.insert(std::make_pair(info.id, cloth));

		return true;
	}
}