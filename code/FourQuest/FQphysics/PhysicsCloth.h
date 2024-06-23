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

namespace fq::physics
{
	struct Vertex
	{
		DirectX::SimpleMath::Vector3 Pos;
		DirectX::SimpleMath::Vector3 Normal;
		DirectX::SimpleMath::Vector3 Tangent;
		DirectX::SimpleMath::Vector2 Tex;
	};

	class PhysicsCloth
	{
	public:
		PhysicsCloth(unsigned int id, unsigned int layerNumber);
		~PhysicsCloth();

		bool Initialize(const PhysicsClothInfo& info, physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager);
		bool UpdatePhysicsCloth(physx::PxCudaContextManager* cudaContextManager);

		void GetPhysicsCloth(PhysicsClothGetData& data);
		bool SetPhysicsCloth(const PhysicsClothSetData& data);

		bool RegisterD3D11VertexBufferWithCUDA(ID3D11Buffer* buffer);
		bool RegisterD3D11IndexBufferWithCUDA(ID3D11Buffer* buffer);

	private:
		void settingInfoData(const PhysicsClothInfo& info);
		void extractSpringsData();
		void createClothParticle(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxCudaContextManager* cudaContextManager);

		void settingParticleBuffer(
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

#pragma region CudaFunction
		bool updatePhysXDataToID3DBuffer(
			std::vector<DirectX::SimpleMath::Vector3>& vertices,
			std::vector<unsigned int>& indices,
			std::vector<DirectX::SimpleMath::Vector2> uvs,
			cudaGraphicsResource* directXVertexBuffer,
			physx::PxVec4* particle);

		bool updateNormalToID3DBuffer(
			std::vector<std::pair<unsigned int, unsigned int>>& sameVertices,
			unsigned int vertexSize,
			cudaGraphicsResource* directXVertexBuffer);

		bool updateWorldTransformToID3DBuffer(
			const DirectX::SimpleMath::Matrix& prevWorldTransform,
			const DirectX::SimpleMath::Matrix& nextWorldTrnasform,
			const unsigned int vertexSize,
			physx::PxVec4* particle);
#pragma endregion


	private:
		unsigned int	mID;
		unsigned int	mLayNumber;

		DirectX::SimpleMath::Matrix mWorldTransform;
		std::vector<DirectX::SimpleMath::Vector3> mVertices;
		std::vector<DirectX::SimpleMath::Vector2> mUV;
		float mTotalClothMass;
		std::vector<unsigned int> mIndices;
		std::set<std::pair<unsigned int, unsigned int>> mSprings;
		std::vector<std::pair<unsigned int, unsigned int>> mSameVertices;

		physx::PxPBDMaterial* mPBDMaterial;
		physx::PxPBDParticleSystem* mParticleSystem;
		physx::PxParticleClothBuffer* mClothBuffer;
		physx::ExtGpu::PxParticleClothBufferHelper* mClothBufferHelper;

		cudaGraphicsResource* mCudaVertexResource;
		cudaGraphicsResource* mCudaIndexResource;
	};

#pragma region GetSet

#pragma endregion

}