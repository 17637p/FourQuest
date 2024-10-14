#pragma once

#include "FQCommonPhysics.h"

#include <memory>
#include <unordered_map>

#include <PxPhysicsAPI.h>

namespace fq::physics 
{
	class CudaPhysicsCloth;
	class PhysicsCollisionDataManager;

	class PhysicsClothManager
	{
	public:
		PhysicsClothManager();
		~PhysicsClothManager();

		/// <summary>
		/// õ �ùķ��̼� �Ŵ��� �ʱ�ȭ�Դϴ�.
		/// </summary>
		bool Initialize(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager);

		/// <summary>
		/// õ �ùķ��̼� ������Ʈ �մϴ�. ( �׷��� ������ ���� )
		/// </summary>
		bool Update();

		/// <summary>
		/// õ�� �����մϴ�.
		/// </summary>
		bool CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix);

		/// <summary>
		/// õ �����͸� ����ϴ�.
		/// </summary>
		bool GetClothData(unsigned int id, Cloth::GetSetClothData& data);

		/// <summary>
		/// õ �����͸� �����մϴ�.
		/// </summary>
		bool SetClothData(unsigned int id, const Cloth::GetSetClothData& data);

		/// <summary>
		/// õ�� �����մϴ�.
		/// </summary>
		bool RemoveCloth(unsigned int id, std::vector<physx::PxActor*>& removeActorList);

	private:
		std::weak_ptr<PhysicsCollisionDataManager> mCollisionDataManager;

		std::unordered_map<unsigned int, std::shared_ptr<CudaPhysicsCloth>> mPhysicsClothContainer;

		physx::PxPhysics* mPhysics;
		physx::PxScene* mScene;
		physx::PxCudaContextManager* mCudaContextManager;
	};
}