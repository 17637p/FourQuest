#pragma once

#include "FQCommonPhysics.h"

#include <memory>
#include <unordered_map>

#include <PxPhysicsAPI.h>

namespace fq::physics 
{
	class PhysicsCloth;

	class PhysicsClothManager
	{
	public:
		PhysicsClothManager();
		~PhysicsClothManager();

		/// <summary>
		/// õ �ùķ��̼� �Ŵ��� �ʱ�ȭ�Դϴ�.
		/// </summary>
		bool Initialize(physx::PxFoundation* foundation, physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager);

		bool CreateCloth(const PhysicsClothInfo& info);

	private:
		std::unordered_map<unsigned int, std::shared_ptr<PhysicsCloth>> mPhysicsClothContainer;

		physx::PxPhysics* mPhysics;
		physx::PxScene* mScene;
		physx::PxCudaContextManager* mCudaContextManager;
	};
}