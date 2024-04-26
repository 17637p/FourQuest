#pragma once

#include <unordered_map>

#include <physx\PxPhysicsAPI.h>
#include <directxtk/SimpleMath.h>

namespace fq::physics
{
	class PhysicsCookingMeshTool
	{
	public:
		PhysicsCookingMeshTool();
		~PhysicsCookingMeshTool();

		/// <summary>
		/// 모델 버텍스 정보들을 읽어서 굽는 툴 클래스를 세팅합니다.
		/// </summary>
		bool Initialize(physx::PxPhysics* physics);

		/// <summary>
		/// 모델 데이터의 버텍스 정보들을 읽어서 ConvexMesh 데이터를 추출합니다.
		/// </summary>
		/// <returns> PxShape에 들어갈 ConvexMesh를 생성하여 반환합니다. </returns>
		physx::PxConvexMesh* CookingConvexMesh(DirectX::SimpleMath::Vector3* vertices, int vertexSize, int vertexLimit);

		/// <summary>
		/// 모델 데이터의 버텍스 포인터 값을 읽어서 해당 포인터의 키 값을 가지고 있는 mConvexMeshMap에 ConvexMesh를 찾아 반환합니다.
		/// </summary>
		/// <returns> 이미 리소스 정보가 있으면 ConvexMesh 반환, 없으면 nullptr을 반환합니다. </returns>
		inline physx::PxConvexMesh* FindConvexMesh(DirectX::SimpleMath::Vector3* vertices);

	private:
		physx::PxPhysics* mPhysics;
		std::unordered_map<DirectX::SimpleMath::Vector3*, physx::PxConvexMesh*> mConvexMeshMap;
	};

	physx::PxConvexMesh* PhysicsCookingMeshTool::FindConvexMesh(DirectX::SimpleMath::Vector3* vertices)
	{
		return  mConvexMeshMap.find(vertices)->second;
	}
}
