#pragma once

#include "FQCommonPhysics.h"

#include <iostream>

#include <d3d11.h>

#include <PxPhysicsAPI.h>
#include <extensions/PxParticleExt.h>
#include <cudamanager\PxCudaContext.h>

#include <cuda_runtime.h>
#include <cuda_d3d11_interop.h>
#include <device_launch_parameters.h>

#include <set>
#include <unordered_map>
#include <unordered_set>

namespace fq::physics
{
	class CudaPhysicsCloth
	{
	public:
		CudaPhysicsCloth(unsigned int id, unsigned int layerNumber);
		~CudaPhysicsCloth();

		/// <summary>
		/// 초기화
		/// </summary>
		bool Initialize(const Cloth::CreateClothData& info,
			physx::PxPhysics* physics, 
			physx::PxScene* scene, 
			physx::PxCudaContextManager* cudaContextManager, 
			std::shared_ptr<CollisionData> collisionData,
			int* collisionMatrix);

		/// <summary>
		/// 실시간 시뮬레이션 된 천 입자들을 ID11Buffer에 업데이트
		/// </summary>
		bool UpdatePhysicsCloth(physx::PxCudaContextManager* cudaContextManager);

		/// <summary>
		/// 천 GetSet
		/// </summary>
		void GetPhysicsCloth(Cloth::GetSetClothData& data);
		bool SetPhysicsCloth(const Cloth::GetSetClothData& data);

		/// <summary>
		/// 버텍스 ID3D11Buffer 저장
		/// </summary>
		bool RegisterD3D11VertexBufferWithCUDA(ID3D11Buffer* buffer);

		/// <summary>
		/// 인덱스 ID3D11Buffer 저장
		/// </summary>
		bool RegisterD3D11IndexBufferWithCUDA(ID3D11Buffer* buffer);

		inline const unsigned int& GetID();
		inline const unsigned int& GetLayerNumber();
		inline const DirectX::SimpleMath::Matrix& GetWorldTrnasform();
		inline const float& GetClothMass();
		inline const float& GetRestOffset();
		inline physx::PxPBDParticleSystem* GetPBDParticleSystem();
		inline void SetLayerNumber(const unsigned int& layerNumber);


	private:
		void settingInfoData(const Cloth::CreateClothData& info);
		void extractSpringsData();
		void createClothParticle(
			const Cloth::CreateClothData& info,
			physx::PxPhysics* physics, 
			physx::PxScene* scene, 
			physx::PxCudaContextManager* cudaContextManager, 
			int* collisionMatrix, 
			std::shared_ptr<CollisionData> collisionData);

		void settingParticleBuffer(
			const Cloth::CreateClothData& info,
			const physx::PxU32& numSprings,
			const physx::PxU32& numTriangles,
			const physx::PxU32& numParticles,
			const physx::PxU32& particlePhase,
			const physx::PxReal& particleMass,
			unsigned int* phase,
			physx::PxVec4* positionInvMass,
			physx::PxVec4* velocity);

		void createCloth(
			const physx::PxU32& numParticles,
			physx::PxCudaContextManager* cudaContextManager,
			unsigned int* phase,
			physx::PxVec4* positionInvMass,
			physx::PxVec4* velocity);

	private:
		unsigned int	mID;
		unsigned int	mLayerNumber;

		DirectX::SimpleMath::Matrix					mWorldTransform;
		std::vector<DirectX::SimpleMath::Vector3>	mVertices;
		std::vector<DirectX::SimpleMath::Vector2>	mUV;
		float mClothMass;
		float mRestOffset;
		std::vector<unsigned int> mIndices;
		std::set<std::pair<unsigned int, unsigned int>>		mSprings;
		std::vector<std::pair<unsigned int, unsigned int>>	mSameVertices;

		physx::PxPBDMaterial*						mPBDMaterial;
		physx::PxPBDParticleSystem*					mParticleSystem;
		physx::PxParticleClothBuffer*				mClothBuffer;
		physx::ExtGpu::PxParticleClothBufferHelper* mClothBufferHelper;

		cudaGraphicsResource* mCudaVertexResource;
		cudaGraphicsResource* mCudaIndexResource;
	};

#pragma region GetSet
	const unsigned int& CudaPhysicsCloth::GetID()
	{
		return mID;
	}
	const unsigned int& CudaPhysicsCloth::GetLayerNumber()
	{
		return mLayerNumber;
	}
	const DirectX::SimpleMath::Matrix& CudaPhysicsCloth::GetWorldTrnasform()
	{
		return mWorldTransform;
	}
	const float& CudaPhysicsCloth::GetClothMass()
	{
		return mClothMass;
	}
	const float& CudaPhysicsCloth::GetRestOffset()
	{
		return mRestOffset;
	}
	physx::PxPBDParticleSystem* CudaPhysicsCloth::GetPBDParticleSystem()
	{
		return mParticleSystem;
	}
	void CudaPhysicsCloth::SetLayerNumber(const unsigned int& layerNumber)
	{
		mLayerNumber = layerNumber;
	}
#pragma endregion

}

