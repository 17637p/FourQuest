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
			DirectX::SimpleMath::Vector3* vertices,
			unsigned int vertexSize,
			unsigned int* indices,
			unsigned int indexSize);

	};
}
