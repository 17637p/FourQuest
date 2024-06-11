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
		/// 천 시뮬레이션 매니저 초기화입니다.
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