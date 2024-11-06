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
		bool Initialize(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager);

		/// <summary>
		/// õ �ùķ��̼��� �����۾��Ͽ� ���ڸ� ������Ʈ �մϴ�. (�׷��� ������ ���� )
		/// </summary>
		bool Update(float deltaTime);

		/// <summary>
		/// ���̳� ������Ʈ (��Ʈ�� ����)
		/// </summary>
		/// <returns></returns>
		bool FinalUpdate();

		/// <summary>
		/// �ùķ��̼ǿ��� ������Ʈ �� ������ ó��
		/// </summary>
		bool UpdateSimulationData(float deltaTime);

		/// <summary>
		/// õ�� �����մϴ�.
		/// </summary>
		bool CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix, bool isSkinnedMesh);
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

		/// <summary>
		/// ��� õ�� �����մϴ�.
		/// </summary>
		bool RemoveAllCloth(std::vector<physx::PxActor*>& removeActorList);

		/// <summary>
		/// õ�� ���ؽ��� ��ȯ�մϴ�.
		/// </summary>
		const std::vector<DirectX::SimpleMath::Vector3>& GetClothVertex(unsigned int id);

		/// <summary>
		/// õ�� �ε����� ��ȯ�մϴ�.
		/// </summary>
		const std::vector<unsigned int>& GetClothIndices(unsigned int id);

	private:
		std::weak_ptr<PhysicsCollisionDataManager> mCollisionDataManager;

		std::unordered_map<unsigned int, std::shared_ptr<CudaPhysicsCloth>> mPhysicsClothContainer;
		std::vector<std::pair<Cloth::CreateClothData, int*>> mUpCommingClothVec;

		physx::PxPhysics* mPhysics;
		physx::PxScene* mScene;
		physx::PxCudaContextManager* mCudaContextManager;
	};
}