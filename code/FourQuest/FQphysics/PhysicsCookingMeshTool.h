#pragma once

#include <unordered_map>

#include <PxPhysicsAPI.h>
#include <directxtk/SimpleMath.h>

namespace fq::physics
{
	class PhysicsCookingMeshTool
	{
	public:
		/// <summary>
		/// 모델 데이터의 버텍스 정보들을 읽어서 ConvexMesh 데이터를 추출합니다.
		/// </summary>
		/// <returns> PxShape에 들어갈 ConvexMesh를 생성하여 반환합니다. </returns>
		static physx::PxConvexMesh* CookingConvexMesh(physx::PxPhysics* physics, DirectX::SimpleMath::Vector3* vertices, int vertexSize, int polygonLimit);

		/// <summary>
		/// 모델 데이터의 인덱스, 버텍스 정보들을 읽어서 물리 공간의 TriangleMesh를 만듭니다.
		/// </summary>
		static physx::PxTriangleMesh* CookingTriangleMesh(
			physx::PxPhysics* physics,
			const DirectX::SimpleMath::Vector3* vertices,
			const unsigned int& vertexSize,
			const unsigned int* indices,
			const unsigned int& indexSize);

		/// <summary>
		/// 터레인의 값을 읽어서 물리 공간에서 물리 시뮬레이션을 돌릴 수 있는 필드 맵을 생서합니다.
		/// </summary>
		static physx::PxHeightField* CookingHeightField(
			physx::PxPhysics* physics,
			const int* height,
			const unsigned int& numCols,
			const unsigned int& numRows);
	};
}
