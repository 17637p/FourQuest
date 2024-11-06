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
		/// 천 시뮬레이션 매니저 초기화입니다.
		/// </summary>
		bool Initialize(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager);

		/// <summary>
		/// 천 시뮬레이션을 보간작업하여 입자를 업데이트 합니다. (그래픽 데이터 전송 )
		/// </summary>
		bool Update(float deltaTime);

		/// <summary>
		/// 파이널 업데이트 (스트림 종료)
		/// </summary>
		/// <returns></returns>
		bool FinalUpdate();

		/// <summary>
		/// 시뮬레이션에서 업데이트 된 데이터 처리
		/// </summary>
		bool UpdateSimulationData(float deltaTime);

		/// <summary>
		/// 천을 생성합니다.
		/// </summary>
		bool CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix, bool isSkinnedMesh);
		bool CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix);

		/// <summary>
		/// 천 데이터를 얻습니다.
		/// </summary>
		bool GetClothData(unsigned int id, Cloth::GetSetClothData& data);

		/// <summary>
		/// 천 데이터를 세팅합니다.
		/// </summary>
		bool SetClothData(unsigned int id, const Cloth::GetSetClothData& data);

		/// <summary>
		/// 천을 삭제합니다.
		/// </summary>
		bool RemoveCloth(unsigned int id, std::vector<physx::PxActor*>& removeActorList);

		/// <summary>
		/// 모든 천을 삭제합니다.
		/// </summary>
		bool RemoveAllCloth(std::vector<physx::PxActor*>& removeActorList);

		/// <summary>
		/// 천의 버텍스를 반환합니다.
		/// </summary>
		const std::vector<DirectX::SimpleMath::Vector3>& GetClothVertex(unsigned int id);

		/// <summary>
		/// 천의 인덱스를 반환합니다.
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