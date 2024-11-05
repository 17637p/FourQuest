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
#include <random>

namespace fq::physics
{

	class CudaPhysicsCloth
	{
	public:
		CudaPhysicsCloth(unsigned int id, unsigned int layerNumber);
		~CudaPhysicsCloth();

		/// <summary>
		/// �ʱ�ȭ
		/// </summary>
		bool Initialize(const Cloth::CreateClothData& info,
			physx::PxPhysics* physics, 
			physx::PxScene* scene, 
			physx::PxCudaContextManager* cudaContextManager, 
			std::shared_ptr<CollisionData> collisionData,
			int* collisionMatrix,
			bool isSkinnedMesh);

		/// <summary>
		/// ���ڵ��� ����ó�� �� ID11Buffer�� ������Ʈ
		/// </summary>
		bool UpdatePhysicsCloth(physx::PxCudaContextManager* cudaContextManager);
		bool UpdatePhysicsCloth(physx::PxCudaContextManager* cudaContextManager, float deltaTime);

		/// <summary>
		/// �ùķ��̼� �� ������Ʈ �� õ ���ڵ��� ��ġ ���� ������Ʈ�մϴ�.
		/// </summary>
		bool UpdateParticleBuffer(float deltaTime);

		/// <summary>
		/// õ GetSet
		/// </summary>
		void GetPhysicsCloth(Cloth::GetSetClothData& data);
		bool SetPhysicsCloth(const Cloth::GetSetClothData& data);

		/// <summary>
		/// ���ؽ� ID3D11Buffer ����
		/// </summary>
		bool RegisterD3D11VertexBufferWithCUDA(ID3D11Buffer* buffer);

		/// <summary>
		/// �ε��� ID3D11Buffer ����
		/// </summary>
		bool RegisterD3D11IndexBufferWithCUDA(ID3D11Buffer* buffer);

		inline const unsigned int& GetID();
		inline const unsigned int& GetLayerNumber();
		inline const bool& GetIsCulling();
		inline const DirectX::SimpleMath::Matrix& GetWorldTrnasform();
		inline const float& GetClothMass();
		inline const float& GetRestOffset();
		inline physx::PxPBDParticleSystem* GetPBDParticleSystem();
		inline const std::vector<DirectX::SimpleMath::Vector3>& GetVertices();
		inline const std::vector<unsigned int>& GetIndices();
		inline void SetLayerNumber(const unsigned int& layerNumber);
		inline void SetIsCulling(const bool& isCulling);


	private:
		bool updateDebugVertex();
		bool updateWindToParticle();
		bool settingInfoData(const Cloth::CreateClothData& info);
		bool extractSpringsData();
		bool createClothParticle(
			physx::PxPhysics* physics, 
			physx::PxScene* scene, 
			physx::PxCudaContextManager* cudaContextManager, 
			int* collisionMatrix, 
			std::shared_ptr<CollisionData> collisionData);

		bool settingParticleBuffer(
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

		// ������ �ٶ� ���� �Լ�
		DirectX::SimpleMath::Vector3 getRandomWindForce(float maxWindStrength);

	private:
		unsigned int	mID;
		unsigned int	mLayerNumber;
		bool mbIsSkinnedMesh;
		bool mbIsCulling;
		std::mt19937 mGen; // �� ��ü�� ������ ���� ����

		DirectX::SimpleMath::Matrix					mWorldTransform;
		std::vector<DirectX::SimpleMath::Vector3>	mVertices;
		std::vector<DirectX::SimpleMath::Vector2>	mUV;
		float mClothMass;
		float mRestOffset;
		float mMaxWindStrength;
		std::vector<unsigned int> mIndices;
		std::vector<unsigned int> mDisableIndicesIndices;
		std::set<std::pair<unsigned int, unsigned int>>		mSprings;
		std::vector<std::pair<unsigned int, unsigned int>>	mSameVertices;

		physx::PxPBDMaterial*						mPBDMaterial;
		physx::PxPBDParticleSystem*					mParticleSystem;
		physx::PxParticleClothBuffer*				mClothBuffer;
		physx::ExtGpu::PxParticleClothBufferHelper* mClothBufferHelper;

		cudaGraphicsResource* mCudaVertexResource;
		cudaGraphicsResource* mCudaIndexResource;
		UINT mCudaVertexStride;

		std::vector<DirectX::SimpleMath::Vector4> mPrevClothBuffer;
		std::vector<DirectX::SimpleMath::Vector4> mCurrClothBuffer;
		float mDurationTime;
		float mLerpTime;
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
	inline const bool& CudaPhysicsCloth::GetIsCulling()
	{
		return mbIsCulling;
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
	inline void CudaPhysicsCloth::SetIsCulling(const bool& isCulling)
	{
		mbIsCulling = isCulling;
	}
	const std::vector<DirectX::SimpleMath::Vector3>& CudaPhysicsCloth::GetVertices()
	{
		return mVertices;
	}
	const std::vector<unsigned int>& CudaPhysicsCloth::GetIndices()
	{
		return mIndices;
	}
#pragma endregion

}

