#include "CudaClothTool.h"

#include <cuda_runtime.h>
#include <cuda_d3d11_interop.h>
#include <device_launch_parameters.h>

#include <iostream>

#include "../FQCommon/FQCommonGraphics.h"

struct SimpleVector3 {
	float x, y, z;
};

struct SimpleMatrix {
	float m[4][4];
};

__device__ physx::PxVec2 Sub(const physx::PxVec2& lhs, const physx::PxVec2& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

__device__ physx::PxVec4 Sub(const physx::PxVec4& lhs, const physx::PxVec4& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}

__device__ physx::PxVec3 cross(const physx::PxVec4& a, const physx::PxVec4& b) {
	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

__device__ float DotProduct(const physx::PxVec3& a, const physx::PxVec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

__device__ void NormalizeVector(physx::PxVec3& vec) {
	float length = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	if (length > 0) {
		vec.x /= length;
		vec.y /= length;
		vec.z /= length;
	}
}

template <typename T>
__device__ SimpleVector3 multiply(SimpleMatrix& mat, const T& vec)
{
	SimpleVector3 result;
	result.x = mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z + mat.m[3][0] * 1.0f;
	result.y = mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z + mat.m[3][1] * 1.0f;
	result.z = mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z + mat.m[3][2] * 1.0f;
	return result;
}

#pragma region UpdateID3D11VertexBuffer
// CUDA 커널 함수 정의
template <typename T>
__global__ void UpdateVertex(
	physx::PxVec4* vertices,
	physx::PxVec2* uvs,
	unsigned int vertexSize,
	unsigned int* indices,
	unsigned int indexSize,
	SimpleMatrix invTransform,
	T* buffer)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= indexSize / 3) return;

	unsigned int i0 = indices[idx * 3];
	unsigned int i1 = indices[idx * 3 + 1];
	unsigned int i2 = indices[idx * 3 + 2];

	physx::PxVec4 v0 = vertices[i0];
	physx::PxVec4 v1 = vertices[i1];
	physx::PxVec4 v2 = vertices[i2];

	physx::PxVec2 uv0 = uvs[i0];
	physx::PxVec2 uv1 = uvs[i1];
	physx::PxVec2 uv2 = uvs[i2];

	physx::PxVec4 edge1 = Sub(v1, v0);
	physx::PxVec4 edge2 = Sub(v2, v0);

	physx::PxVec2 deltaUV1 = Sub(uv1, uv0);
	physx::PxVec2 deltaUV2 = Sub(uv2, uv0);

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	physx::PxVec3 tangent;
	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	NormalizeVector(tangent);

	physx::PxVec3 bitangent;
	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	NormalizeVector(bitangent);

	physx::PxVec3 normal = cross(edge1, edge2);
	NormalizeVector(normal);

	SimpleVector3 vertex0;
	SimpleVector3 vertex1;
	SimpleVector3 vertex2;
	vertex0.x = v0.x;
	vertex0.y = v0.y;
	vertex0.z = v0.z;
	vertex1.x = v1.x;
	vertex1.y = v1.y;
	vertex1.z = v1.z;
	vertex2.x = v2.x;
	vertex2.y = v2.y;
	vertex2.z = v2.z;
	vertex0 = multiply< SimpleVector3>(invTransform, vertex0);
	vertex1 = multiply< SimpleVector3>(invTransform, vertex1);
	vertex2 = multiply< SimpleVector3>(invTransform, vertex2);

	buffer[i0].Pos.x = vertex0.x;
	buffer[i0].Pos.y = vertex0.y;
	buffer[i0].Pos.z = vertex0.z;
	buffer[i1].Pos.x = vertex1.x;
	buffer[i1].Pos.y = vertex1.y;
	buffer[i1].Pos.z = vertex1.z;
	buffer[i2].Pos.x = vertex2.x;
	buffer[i2].Pos.y = vertex2.y;
	buffer[i2].Pos.z = vertex2.z;

	buffer[i0].Normal.x = normal.x;
	buffer[i0].Normal.y = normal.y;
	buffer[i0].Normal.z = normal.z;
	buffer[i1].Normal.x = normal.x;
	buffer[i1].Normal.y = normal.y;
	buffer[i1].Normal.z = normal.z;
	buffer[i2].Normal.x = normal.x;
	buffer[i2].Normal.y = normal.y;
	buffer[i2].Normal.z = normal.z;

	buffer[i0].Tangent.x = tangent.x;
	buffer[i0].Tangent.y = tangent.y;
	buffer[i0].Tangent.z = tangent.z;
	buffer[i1].Tangent.x = tangent.x;
	buffer[i1].Tangent.y = tangent.y;
	buffer[i1].Tangent.z = tangent.z;
	buffer[i2].Tangent.x = tangent.x;
	buffer[i2].Tangent.y = tangent.y;
	buffer[i2].Tangent.z = tangent.z;
}

template <typename T>
__global__ void UpdateVertexNoIndex(
	physx::PxVec4* vertices,
	unsigned int vertexSize,
	SimpleMatrix invTransform,
	T* buffer)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= vertexSize) return;

	// 해당 정점을 가져옵니다
	physx::PxVec4 v = vertices[idx];

	// 변환을 위한 SimpleVector3 생성
	SimpleVector3 vertex;
	vertex.x = v.x;
	vertex.y = v.y;
	vertex.z = v.z;

	// 변환 행렬을 적용하여 정점 위치 업데이트
	vertex = multiply<SimpleVector3>(invTransform, vertex);

	// 변환된 정점 위치를 buffer에 업데이트
	buffer[idx].Pos.x = vertex.x;
	buffer[idx].Pos.y = vertex.y;
	buffer[idx].Pos.z = vertex.z;
}

template <typename T>
__global__ void UpdateLerpVertexNoIndex(
	physx::PxVec4* prevVertices,
	physx::PxVec4* currVertices,
	float t,
	unsigned int vertexSize,
	SimpleMatrix invTransform,
	T* buffer)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= vertexSize) return;

	// 해당 정점을 가져옵니다
	physx::PxVec4 prevV = prevVertices[idx];
	physx::PxVec4 currV = currVertices[idx];

	// 변환을 위한 SimpleVector3 생성 ( 이전 입자 위치, 최근 입자 위치 보간 계산 )
	SimpleVector3 vertex;
	vertex.x = prevV.x + t * (currV.x - prevV.x);
	vertex.y = prevV.y + t * (currV.y - prevV.y);
	vertex.z = prevV.z + t * (currV.z - prevV.z);

	// 변환 행렬을 적용하여 정점 위치 업데이트
	vertex = multiply<SimpleVector3>(invTransform, vertex);

	// 변환된 정점 위치를 buffer에 업데이트
	buffer[idx].Pos.x = vertex.x;
	buffer[idx].Pos.y = vertex.y;
	buffer[idx].Pos.z = vertex.z;
}
#pragma endregion

#pragma region SetID3D11BufferVertexNormal
template <typename T>
__global__ void processVerticesKernel(unsigned int* sameVerticesFirst, unsigned int* sameVerticesSecond, T* buffer, int size)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	int first = sameVerticesFirst[idx];
	int second = sameVerticesSecond[idx];

	if (first < size && second < size)
	{
		// 중간값을 사용하여 덧셈 연산을 분리
		float3 normalTemp, tangentTemp, biTangentTemp;

		normalTemp.x = buffer[first].Normal.x + buffer[second].Normal.x;
		normalTemp.y = buffer[first].Normal.y + buffer[second].Normal.y;
		normalTemp.z = buffer[first].Normal.z + buffer[second].Normal.z;

		tangentTemp.x = buffer[first].Tangent.x + buffer[second].Tangent.x;
		tangentTemp.y = buffer[first].Tangent.y + buffer[second].Tangent.y;
		tangentTemp.z = buffer[first].Tangent.z + buffer[second].Tangent.z;

		buffer[first].Normal.x = normalTemp.x;
		buffer[first].Normal.y = normalTemp.y;
		buffer[first].Normal.z = normalTemp.z;
		buffer[second].Normal.x = normalTemp.x;
		buffer[second].Normal.y = normalTemp.y;
		buffer[second].Normal.z = normalTemp.z;

		buffer[first].Tangent.x = tangentTemp.x;
		buffer[first].Tangent.y = tangentTemp.y;
		buffer[first].Tangent.z = tangentTemp.z;
		buffer[second].Tangent.x = tangentTemp.x;
		buffer[second].Tangent.y = tangentTemp.y;
		buffer[second].Tangent.z = tangentTemp.z;
	}
}
#pragma endregion

#pragma region SetClothParticleWorldTransform

// 커널 함수
__global__ void TransformVertices(
	physx::PxVec4* particle,
	SimpleMatrix previousTransformInverse,
	SimpleMatrix newTransform,
	int vertexCount)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= vertexCount) return;

	SimpleVector3 vertex;
	vertex.x = particle[idx].x;
	vertex.y = particle[idx].y;
	vertex.z = particle[idx].z;

	// 이전 worldTransform의 역행렬 적용
	vertex = multiply< SimpleVector3>(previousTransformInverse, vertex);

	// 새로운 worldTransform 적용
	vertex = multiply< SimpleVector3>(newTransform, vertex);

	// 변환된 vertex 저장
	particle[idx].x = vertex.x;
	particle[idx].y = vertex.y;
	particle[idx].z = vertex.z;
}
#pragma endregion

#pragma region CopyFromGPUToCPU
template <typename T>
__global__ void CopyVertexDataToCPU(
	physx::PxVec3* vertices,
	physx::PxVec2* uvs,
	SimpleMatrix worldTransform,
	T* ID3D11VertexBuffer,
	unsigned int vertexSize)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= vertexSize) return;

	SimpleVector3 vertex;

	vertex.x = ID3D11VertexBuffer[idx].Pos.x;
	vertex.y = ID3D11VertexBuffer[idx].Pos.y;
	vertex.z = ID3D11VertexBuffer[idx].Pos.z;

	uvs[idx].x = ID3D11VertexBuffer[idx].Tex.x;
	uvs[idx].y = ID3D11VertexBuffer[idx].Tex.y;

	vertex = multiply(worldTransform, vertex);

	vertices[idx].x = vertex.x;
	vertices[idx].y = vertex.y;
	vertices[idx].z = vertex.z;
}
#pragma endregion

#pragma region UpdateSkinnedMeshToPhysXVertex
__global__ void UpdateVertices(Vertex* vertices, physx::PxVec4* particleData, size_t vertexCount)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx < vertexCount)
	{
		// 애니메이션 데이터를 기반으로 정점 좌표 업데이트
		particleData[idx].x = vertices[idx].Pos.x;
		particleData[idx].y = vertices[idx].Pos.y;
		particleData[idx].z = vertices[idx].Pos.z;
	}
}
#pragma endregion

#pragma region SetSimulation
__global__ void SetSimulationStopVertex(physx::PxVec4* particleData, size_t vertexCount)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx < vertexCount)
	{
		// 애니메이션 데이터를 기반으로 정점 좌표 업데이트
		particleData[idx].w = 0.f;
	}
}
#pragma endregion

namespace fq::physics
{
	bool CudaClothTool::copyVertexFromGPUToCPU(
		std::vector<DirectX::SimpleMath::Vector3>& vertices,
		std::vector<DirectX::SimpleMath::Vector2>& uvs,
		DirectX::SimpleMath::Matrix& worldTransform,
		cudaGraphicsResource* ID3D11VertexBuffer,
		UINT ID3D11VertexStride)
	{
		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &ID3D11VertexBuffer); 
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyVertexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t vertexSize = 0;
		cudaStatus = cudaGraphicsResourceGetMappedPointer(&devPtr, &vertexSize, ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyVertexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		int threadsPerBlock = 256;
		int blocksPerGrid = (vertexSize + threadsPerBlock - 1) / threadsPerBlock;

		// GPU 메모리 할당
		physx::PxVec3* d_vertices;
		physx::PxVec2* d_uvs;
		cudaMalloc(&d_vertices, vertexSize * sizeof(DirectX::SimpleMath::Vector3));
		cudaMalloc(&d_uvs, vertexSize * sizeof(DirectX::SimpleMath::Vector2));

		SimpleMatrix Trnasform;
		memcpy(&Trnasform, &worldTransform, sizeof(SimpleMatrix));

		// CUDA 함수 실행
		if (ID3D11VertexStride == 44)
		{
			CopyVertexDataToCPU <Vertex><< < blocksPerGrid, threadsPerBlock >> > (d_vertices, d_uvs, Trnasform, (Vertex*)devPtr, vertexSize);
		}
		else if (ID3D11VertexStride == 52)
		{
			CopyVertexDataToCPU <Vertex1> << < blocksPerGrid, threadsPerBlock >> > (d_vertices, d_uvs, Trnasform, (Vertex1*)devPtr, vertexSize);
		}
		else if (ID3D11VertexStride == 60)
		{
			CopyVertexDataToCPU <Vertex2> << < blocksPerGrid, threadsPerBlock >> > (d_vertices, d_uvs, Trnasform, (Vertex2*)devPtr, vertexSize);
		}
		
		cudaStatus = cudaDeviceSynchronize(); 
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyVertexFromGPUToCPU Error(Error Code : "<< cudaStatus << ")" << std::endl;
			return false;
		}

		// 복사한 데이터 값을 CPU 메모리로 복사
		cudaMemcpy(vertices.data(), d_vertices, vertexSize * sizeof(DirectX::SimpleMath::Vector3), cudaMemcpyKind::cudaMemcpyDeviceToHost);
		cudaMemcpy(uvs.data(), d_uvs, vertexSize * sizeof(DirectX::SimpleMath::Vector2), cudaMemcpyKind::cudaMemcpyDeviceToHost);

		cudaStatus = cudaGraphicsUnmapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyVertexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		cudaFree(d_vertices);
		cudaFree(d_uvs);

		return true;
	}

	bool CudaClothTool::copyIndexFromGPUToCPU(
		std::vector<unsigned int>& indices, 
		cudaGraphicsResource* ID3D11IndexBuffer)
	{
		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &ID3D11IndexBuffer); 
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t indexSize = 0;
		cudaStatus = cudaGraphicsResourceGetMappedPointer(&devPtr, &indexSize, ID3D11IndexBuffer); 
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		cudaMemcpy(indices.data(), devPtr, indexSize * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyDeviceToHost);

		cudaStatus = cudaGraphicsUnmapResources(1, &ID3D11IndexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		return true;
	}

	bool CudaClothTool::UpdatePhysXDataToID3DBuffer(
		std::vector<DirectX::SimpleMath::Vector3>& vertices, 
		std::vector<unsigned int>& indices, 
		std::vector<DirectX::SimpleMath::Vector2> uvs, 
		DirectX::SimpleMath::Matrix transform,
		cudaGraphicsResource* ID3D11VertexBuffer,
		UINT ID3D11VertexStride,
		physx::PxVec4* particle)
	{
		int threadsPerBlock = 256;
		int blocksPerGrid = (indices.size() / 3 + threadsPerBlock - 1) / threadsPerBlock;

		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error : " << cudaGetErrorString(cudaStatus) << ")" << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t size = 0;
		cudaStatus = cudaGraphicsResourceGetMappedPointer(&devPtr, &size, ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error : " << cudaGetErrorString(cudaStatus) << ")" << std::endl;
			return false;
		}

		unsigned int vertexSize = vertices.size();
		unsigned int indexSize = indices.size();

		if (size < sizeof(Vertex) * vertexSize) 
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] Mapped size is smaller than expected!" << std::endl;
			return false;
		}

		// GPU Memory를 할당할 변수
		physx::PxVec2* d_uvs;
		unsigned int* d_indices;

		// GPU Memory에 할당 및 데이터 복사
		cudaMalloc(&d_uvs, uvs.size() * sizeof(DirectX::SimpleMath::Vector2));
		cudaMalloc(&d_indices, indices.size() * sizeof(unsigned int));
		cudaMemcpy(d_uvs, uvs.data(), uvs.size() * sizeof(DirectX::SimpleMath::Vector2), cudaMemcpyKind::cudaMemcpyHostToDevice);
		cudaMemcpy(d_indices, indices.data(), indices.size() * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyHostToDevice);

		SimpleMatrix invTransform;
		std::memcpy(&invTransform, &transform, sizeof(invTransform));

		// CUDA 함수 실행
		if (ID3D11VertexStride == 44)
		{
			UpdateVertex <Vertex> << <blocksPerGrid, threadsPerBlock >> > (
				particle, d_uvs, vertexSize, d_indices, indexSize, invTransform, (Vertex*)devPtr);
		}
		else if (ID3D11VertexStride == 52)
		{
			UpdateVertex <Vertex1> << <blocksPerGrid, threadsPerBlock >> > (
				particle, d_uvs, vertexSize, d_indices, indexSize, invTransform, (Vertex1*)devPtr);
		}
		else if (ID3D11VertexStride == 60)
		{
			UpdateVertex <Vertex2> << <blocksPerGrid, threadsPerBlock >> > (
				particle, d_uvs, vertexSize, d_indices, indexSize, invTransform, (Vertex2*)devPtr);
		}

		cudaStatus = cudaDeviceSynchronize();
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// CUDA 리소스를 언매핑
		cudaStatus = cudaGraphicsUnmapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// 메모리 해제
		cudaFree(d_uvs);
		cudaFree(d_indices);

		return true;
	}

	bool CudaClothTool::UpdatePhysXDataToID3DVertexBuffer(
		std::vector<DirectX::SimpleMath::Vector3>& vertices, 
		DirectX::SimpleMath::Matrix transform, 
		cudaGraphicsResource* ID3D11VertexBuffer,
		UINT ID3D11VertexStride, 
		physx::PxVec4* particle)
	{
		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error : " << cudaGetErrorString(cudaStatus) << ")" << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t size = 0;
		cudaStatus = cudaGraphicsResourceGetMappedPointer(&devPtr, &size, ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error : " << cudaGetErrorString(cudaStatus) << ")" << std::endl;
			return false;
		}

		SimpleMatrix invTransform;
		std::memcpy(&invTransform, &transform, sizeof(invTransform));
		unsigned int vertexSize = vertices.size();

		int threadsPerBlock = 256;
		int blocksPerGrid = (vertexSize + threadsPerBlock - 1) / threadsPerBlock;

		// CUDA 함수 실행
		if (ID3D11VertexStride == 44)
		{
			UpdateVertexNoIndex <Vertex> << <blocksPerGrid, threadsPerBlock >> > (
				particle, vertexSize, invTransform, (Vertex*)devPtr);
		}
		else if (ID3D11VertexStride == 52)
		{
			UpdateVertexNoIndex <Vertex1> << <blocksPerGrid, threadsPerBlock >> > (
				particle, vertexSize, invTransform, (Vertex1*)devPtr);
		}
		else if (ID3D11VertexStride == 60)
		{
			UpdateVertexNoIndex <Vertex2> << <blocksPerGrid, threadsPerBlock >> > (
				particle, vertexSize, invTransform, (Vertex2*)devPtr);
		}

		cudaStatus = cudaDeviceSynchronize();
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// CUDA 리소스를 언매핑
		cudaStatus = cudaGraphicsUnmapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		return true;
	}

	bool CudaClothTool::UpdateNormalToID3DBuffer(
		std::vector<std::pair<unsigned int, unsigned int>>& sameVertices,
		unsigned int vertexSize,
		cudaGraphicsResource* ID3D11VertexBuffer,
		UINT ID3D11VertexStride)
	{
		int threadsPerBlock = 256;
		int blocksPerGrid = (sameVertices.size() + threadsPerBlock - 1) / threadsPerBlock;

		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &ID3D11VertexBuffer); 
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateNormalToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t size = 0;
		cudaStatus = cudaGraphicsResourceGetMappedPointer(&devPtr, &size, ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateNormalToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// 같은 위치의 버텍스 세팅
		std::vector<unsigned int> firstVertex;
		std::vector<unsigned int> secondVertex;
		firstVertex.resize(sameVertices.size());
		secondVertex.resize(sameVertices.size());
		for (int i = 0; i < sameVertices.size(); i++)
		{
			firstVertex[i] = sameVertices[i].first;
			secondVertex[i] = sameVertices[i].second;
		}
		
		// GPU 메모리 할당 후 CPU 메모리를 GPU에 복사
		unsigned int* d_firstVertex;
		unsigned int* d_secondVertex;
		cudaMalloc(&d_firstVertex, firstVertex.size() * sizeof(unsigned int));
		cudaMalloc(&d_secondVertex, secondVertex.size() * sizeof(unsigned int));
		cudaMemcpy(d_firstVertex, firstVertex.data(), firstVertex.size() * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyHostToDevice);
		cudaMemcpy(d_secondVertex, secondVertex.data(), secondVertex.size() * sizeof(unsigned int), cudaMemcpyKind::cudaMemcpyHostToDevice);

		// CUDA 함수 실행
		if (ID3D11VertexStride == 44)
		{
			processVerticesKernel <Vertex> << <blocksPerGrid, threadsPerBlock >> > (d_firstVertex, d_secondVertex, (Vertex*)devPtr, vertexSize);
		}
		else if (ID3D11VertexStride == 52)
		{
			processVerticesKernel <Vertex1> << <blocksPerGrid, threadsPerBlock >> > (d_firstVertex, d_secondVertex, (Vertex1*)devPtr, vertexSize);
		}
		else if (ID3D11VertexStride == 60)
		{
			processVerticesKernel <Vertex2> << <blocksPerGrid, threadsPerBlock >> > (d_firstVertex, d_secondVertex, (Vertex2*)devPtr, vertexSize);
		}

		cudaStatus = cudaDeviceSynchronize(); 
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateNormalToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}


		// CUDA 리소스를 언매핑
		cudaStatus = cudaGraphicsUnmapResources(1, &ID3D11VertexBuffer); 
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateNormalToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// 메모리 해제
		cudaFree(d_firstVertex);
		cudaFree(d_secondVertex);

		return true;
	}

	bool CudaClothTool::UpdateWorldTransformToID3DBuffer(
		const DirectX::SimpleMath::Matrix& prevWorldTransform,
		const DirectX::SimpleMath::Matrix& nextWorldTrnasform,
		const unsigned int vertexSize,
		physx::PxVec4* particle)
	{
		int threadsPerBlock = 256;
		int blocksPerGrid = (vertexSize + threadsPerBlock - 1) / threadsPerBlock;


		DirectX::SimpleMath::Matrix prevTransform = prevWorldTransform.Invert();
		DirectX::SimpleMath::Matrix nextTransform = nextWorldTrnasform;

		SimpleMatrix prevMatrix;
		SimpleMatrix nextMatrix;

		std::memcpy(&prevMatrix, &prevTransform, sizeof(prevMatrix));
		std::memcpy(&nextMatrix, &nextTransform, sizeof(prevMatrix));

		TransformVertices << <blocksPerGrid, threadsPerBlock >> > (particle, prevMatrix, nextMatrix, vertexSize);
		cudaError_t cudaStatus = cudaDeviceSynchronize();
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateWorldTransformToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		return true;
	}
	bool CudaClothTool::UpdateSkinnedAnimationVertexToPhysicsVertex(
		cudaGraphicsResource* ID3D11VertexBuffer, 
		physx::PxVec4* particle, 
		unsigned int vertexSize)
	{
		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateNormalToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t size = 0;
		cudaStatus = cudaGraphicsResourceGetMappedPointer(&devPtr, &size, ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateNormalToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// 블록 갯수, 스레드 갯수 설정
		int threadsPerBlock = 256;
		int blocksPerGrid = (vertexSize + threadsPerBlock - 1) / threadsPerBlock;

		// CUDA 함수 실행
		UpdateVertices << <blocksPerGrid, threadsPerBlock >> > ((Vertex*)devPtr, particle, vertexSize);

		cudaStatus = cudaDeviceSynchronize();
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateNormalToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}


		// CUDA 리소스를 언매핑
		cudaStatus = cudaGraphicsUnmapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateNormalToID3DBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		return true;
	}

	bool CudaClothTool::UpdateParticleBuffer(const unsigned int vertexSize, DirectX::SimpleMath::Vector4* currParticle, physx::PxVec4* particle)
	{
		// 복사한 데이터 값을 CPU 메모리로 복사
		cudaError_t cudaStatus = cudaMemcpy(currParticle, particle, vertexSize * sizeof(DirectX::SimpleMath::Vector4), cudaMemcpyKind::cudaMemcpyDeviceToHost);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] UpdateParticleBuffer Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}
		return true;
	}

	bool CudaClothTool::UpdatePhysXDataToID3DVertexBuffer(
		std::vector<DirectX::SimpleMath::Vector4>& prevVertices,
		std::vector<DirectX::SimpleMath::Vector4>& currVertices,
		float deltaTime,
		DirectX::SimpleMath::Matrix invTransform,
		cudaGraphicsResource* ID3D11VertexBuffer,
		UINT ID3D11VertexStride)
	{
		// CUDA 리소스를 매핑
		cudaError_t cudaStatus = cudaGraphicsMapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error : " << cudaGetErrorString(cudaStatus) << ")" << std::endl;
			return false;
		}

		// CUDA 포인터 가져오기
		void* devPtr = nullptr;
		size_t size = 0;
		cudaStatus = cudaGraphicsResourceGetMappedPointer(&devPtr, &size, ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error : " << cudaGetErrorString(cudaStatus) << ")" << std::endl;
			return false;
		}

		unsigned int vertexSize = currVertices.size();

		if (size < sizeof(Vertex) * vertexSize)
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] Mapped size is smaller than expected!" << std::endl;
			return false;
		}

		SimpleMatrix invMatrix;

		std::memcpy(&invMatrix, &invTransform, sizeof(invTransform));

		// GPU Memory를 할당할 변수
		physx::PxVec4* d_prevVertices;
		physx::PxVec4* d_currVertices;

		// GPU Memory에 할당 및 데이터 복사
		cudaMalloc(&d_prevVertices, vertexSize * sizeof(physx::PxVec4));
		cudaMalloc(&d_currVertices, vertexSize * sizeof(physx::PxVec4));
		cudaMemcpy(d_prevVertices, prevVertices.data(), prevVertices.size() * sizeof(physx::PxVec4), cudaMemcpyKind::cudaMemcpyHostToDevice);
		cudaMemcpy(d_currVertices, currVertices.data(), currVertices.size() * sizeof(physx::PxVec4), cudaMemcpyKind::cudaMemcpyHostToDevice);

		int threadsPerBlock = 256;
		int blocksPerGrid = (vertexSize + threadsPerBlock - 1) / threadsPerBlock;

		// CUDA 함수 실행
		if (ID3D11VertexStride == 44)
		{
			UpdateLerpVertexNoIndex <Vertex> << <blocksPerGrid, threadsPerBlock >> > (
				d_prevVertices, d_currVertices, deltaTime, vertexSize, invMatrix, (Vertex*)devPtr);
		}
		else if (ID3D11VertexStride == 52)
		{
			UpdateLerpVertexNoIndex <Vertex1> << <blocksPerGrid, threadsPerBlock >> > (
				d_prevVertices, d_currVertices, deltaTime, vertexSize, invMatrix, (Vertex1*)devPtr);
		}
		else if (ID3D11VertexStride == 60)
		{
			UpdateLerpVertexNoIndex <Vertex2> << <blocksPerGrid, threadsPerBlock >> > (
				d_prevVertices, d_currVertices, deltaTime, vertexSize, invMatrix, (Vertex2*)devPtr);
		}

		cudaStatus = cudaDeviceSynchronize();
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// CUDA 리소스를 언매핑
		cudaStatus = cudaGraphicsUnmapResources(1, &ID3D11VertexBuffer);
		if (!(cudaStatus == cudaSuccess))
		{
			std::cerr << "[CudaClothTool(" << __LINE__ << ")] copyIndexFromGPUToCPU Error(Error Code : " << cudaStatus << ")" << std::endl;
			return false;
		}

		// 메모리 해제
		cudaFree(d_prevVertices);
		cudaFree(d_currVertices);

		return true;
	}
}

