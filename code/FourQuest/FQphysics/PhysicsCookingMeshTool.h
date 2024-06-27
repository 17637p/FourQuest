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

		/// <summary>
		/// �� �������� �ε���, ���ؽ� �������� �о ���� ������ TriangleMesh�� ����ϴ�.
		/// </summary>
		static physx::PxTriangleMesh* CookingTriangleMesh(
			physx::PxPhysics* physics,
			const DirectX::SimpleMath::Vector3* vertices,
			const unsigned int& vertexSize,
			const unsigned int* indices,
			const unsigned int& indexSize);

		/// <summary>
		/// �ͷ����� ���� �о ���� �������� ���� �ùķ��̼��� ���� �� �ִ� �ʵ� ���� �����մϴ�.
		/// </summary>
		static physx::PxHeightField* CookingHeightField(
			physx::PxPhysics* physics,
			const int* height,
			const unsigned int& numCols,
			const unsigned int& numRows);
	};
}
