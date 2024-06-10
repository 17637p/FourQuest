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

	bool PhysicsClothManager::Initialize(physx::PxFoundation* foundation, physx::PxPhysics* physics, physx::PxScene* scene)
	{
		mPhysics = physics;
		mScene = scene;

		if (PxGetSuggestedCudaDeviceOrdinal(foundation->getErrorCallback()) >= 0)
		{
			// initialize CUDA
			physx::PxCudaContextManagerDesc cudaContextManagerDesc;
			mCudaContextManager = PxCreateCudaContextManager(*foundation, cudaContextManagerDesc, PxGetProfilerCallback());
			if (mCudaContextManager && !mCudaContextManager->contextIsValid())
			{
				mCudaContextManager->release();
				mCudaContextManager = NULL;
			}
		}
		if (mCudaContextManager == NULL)
		{
			PxGetFoundation().error(physx::PxErrorCode::eINVALID_OPERATION, PX_FL, "Failed to initialize CUDA!\n");
			return false;
		}

		return true;
	}
	bool PhysicsClothManager::CreateCloth(const PhysicsClothInfo& info)
	{
		std::shared_ptr<PhysicsCloth> cloth = std::make_shared<PhysicsCloth>(info.id, info.layerNumber);

		if (!cloth->Initialize(info, mCudaContextManager, mPhysics))
			return false;

		mPhysicsClothContainer.insert(std::make_pair(info.id, cloth));

		return true;
	}
}