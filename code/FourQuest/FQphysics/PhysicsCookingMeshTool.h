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
	};
}
