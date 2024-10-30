#pragma once

#include <vector>
#include <directxtk\SimpleMath.h>
#include <PxPhysicsAPI.h>
#include <cuda_runtime.h>

struct Vertex
{
	DirectX::SimpleMath::Vector3 Pos;
	DirectX::SimpleMath::Vector3 Normal;
	DirectX::SimpleMath::Vector3 Tangent;
	DirectX::SimpleMath::Vector2 Tex;
	DirectX::SimpleMath::Vector2 Tex1;
};

namespace fq::physics
{
	class CudaClothTool
	{
	public:
		static bool copyVertexFromGPUToCPU(
			std::vector<DirectX::SimpleMath::Vector3>& vertices,
			std::vector<DirectX::SimpleMath::Vector2>& uvs,
			DirectX::SimpleMath::Matrix& worldTransform,
			cudaGraphicsResource* ID3D11VertexBuffer);

		static bool copyIndexFromGPUToCPU(
			std::vector<unsigned int>& indices,
			cudaGraphicsResource* ID3D11VertexBuffer);

		static bool UpdatePhysXDataToID3DBuffer(
			std::vector<DirectX::SimpleMath::Vector3>& vertices,
			std::vector<unsigned int>& indices,
			std::vector<DirectX::SimpleMath::Vector2> uvs,
			cudaGraphicsResource* ID3D11VertexBuffer,
			physx::PxVec4* particle);

		static bool UpdateNormalToID3DBuffer(
			std::vector<std::pair<unsigned int, unsigned int>>& sameVertices,
			unsigned int vertexSize,
			cudaGraphicsResource* ID3D11VertexBuffer);

		static bool UpdateWorldTransformToID3DBuffer(
			const DirectX::SimpleMath::Matrix& prevWorldTransform,
			const DirectX::SimpleMath::Matrix& nextWorldTrnasform,
			const unsigned int vertexSize,
			physx::PxVec4* particle);

		static bool UpdateSkinnedAnimationVertexToPhysicsVertex(
			cudaGraphicsResource* ID3D11VertexBuffer,
			physx::PxVec4* particle,
			unsigned int vertexSize);
	};
}