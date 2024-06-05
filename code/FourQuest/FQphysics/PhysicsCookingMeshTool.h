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
		/// �� �������� ���ؽ� �������� �о ConvexMesh �����͸� �����մϴ�.
		/// </summary>
		/// <returns> PxShape�� �� ConvexMesh�� �����Ͽ� ��ȯ�մϴ�. </returns>
		static physx::PxConvexMesh* CookingConvexMesh(physx::PxPhysics* physics, DirectX::SimpleMath::Vector3* vertices, int vertexSize, int polygonLimit);
	};
}
