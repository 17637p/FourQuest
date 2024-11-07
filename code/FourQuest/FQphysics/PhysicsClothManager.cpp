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
		// 비동기로 쿠다 함수 실행
		for (auto [id, cloth] : mPhysicsClothContainer)
		{
			if (!cloth->UpdatePhysicsCloth(mCudaContextManager, deltaTime)) return false;
		}

		return true;
	}

	bool PhysicsClothManager::FinalUpdate()
	{
		// 비동기 종료
		for (auto [id, cloth] : mPhysicsClothContainer)
		{
			if (!cloth->EndCudaStream()) return true;
		}

		return true;
	}

	bool PhysicsClothManager::UpdateSimulationData(float deltaTime)
	{
		// 천 객체 중에서 현재 프레임의 입자 데이터와 시간을 갱신
		for (auto cloth : mPhysicsClothContainer)
		{
			if (!cloth.second->UpdateParticleBuffer(deltaTime)) return false;
		}

		// PxScene에서 시뮬레이션이 끝나고 난 뒤에 천 시뮬레이션 객체 생성 및 등록
		for (auto [clothData, collisionMatrix] : mUpCommingClothVec)
		{
			CreateCloth(clothData, collisionMatrix);
		}
		mUpCommingClothVec.clear();

		return true;
	}

	bool PhysicsClothManager::CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix, bool isSkinnedMesh)
	{
		// PxScene에 생성할 객체 생성 데이터 등록
		mUpCommingClothVec.push_back(std::make_pair(info, collisionMatrix));
 		return true;
	}

	bool PhysicsClothManager::CreateCloth(const Cloth::CreateClothData& info, int* collisionMatrix)
	{
		std::shared_ptr<CudaPhysicsCloth> cloth = std::make_shared<CudaPhysicsCloth>(info.id, info.layerNumber);
		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();

		// 천 객체 생성
		if (!cloth->Initialize(info, mPhysics, mScene, mCudaContextManager, collisionData, collisionMatrix, false))
		{
			spdlog::error("[PhysicsClothManager ({})] Failed Create Cloth", __LINE__);
			return false;
		}

		// 컨테이너 등록
		mPhysicsClothContainer.insert(std::make_pair(info.id, cloth));
		// 콜리전 데이터 등록
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
		// 천 삭제
		auto clothIter = mPhysicsClothContainer.find(id);
		if (clothIter != mPhysicsClothContainer.end())
		{
			mScene->removeActor(*(clothIter->second->GetPBDParticleSystem()));
			mPhysicsClothContainer.erase(clothIter);

			return true;
		}

		return false;
	}
	bool PhysicsClothManager::RemoveAllCloth(std::vector<physx::PxActor*>& removeActorList)
	{
		// 모든 천 삭제
		for (auto [id, actor] : mPhysicsClothContainer)
		{
			mScene->removeActor(*actor->GetPBDParticleSystem());
		}
		mPhysicsClothContainer.clear();
		mUpCommingClothVec.clear();

		return true;
	}

	const std::vector<DirectX::SimpleMath::Vector4>& PhysicsClothManager::GetClothVertex(unsigned int id)
	{
		auto clothIter = mPhysicsClothContainer.find(id);

		if (clothIter != mPhysicsClothContainer.end())
		{
			auto cloth = clothIter->second;
			return cloth->GetVertices();
		}

		return std::vector<DirectX::SimpleMath::Vector4>();
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