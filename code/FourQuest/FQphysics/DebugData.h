#pragma once

#include <memory>
#include <vector>

#include <directxtk\SimpleMath.h>
#include <PxPhysicsAPI.h>

namespace fq::physics
{
	class RigidBody;

	using PolygonMesh = std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>;

	class DebugData
	{
	public:
		DebugData();
		~DebugData();

		void UpdateDebugData(std::shared_ptr<RigidBody> body);

		inline const std::unordered_map<unsigned int, PolygonMesh>& GetDebugPolygon();
		inline const std::unordered_map<unsigned int, std::vector<unsigned int>>& GetDebugTriangleIndiecs();
		inline const std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>>& GetDebugTriangleVertices();
		inline const std::unordered_map<unsigned int, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>>& GetDebugHeightField();

	private:
		/// <summary>
		/// ConvexMesh 도형을 가진 RigidBody 디버그 데이터 추출
		/// </summary>
		/// <param name="_body"> 리지드 바디 </param>
		/// <param name="_shape"> 도형 </param>
		void extractDebugConvexMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<std::vector<DirectX::SimpleMath::Vector3>>& debugPolygon);

		/// <summary>
		/// TriangleMesh 도형을 가진 RigidBody 디버그 데이터 추출
		/// </summary>
		/// <param name="body"> 리지드 바디 </param>
		/// <param name="shape"> 도형 </param>
		/// <param name="indices"> 인덱스 컨테이너 </param>
		/// <param name="vertices"> 버텍스 컨테이너 </param>
		void extractDebugTriangleMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<unsigned int>& indices, std::vector<DirectX::SimpleMath::Vector3>& vertices);

		/// <summary>
		/// 터레인을 위한 하이트 필드의 디버그 데이터 추출
		/// </summary>
		/// <param name="body"> 리지드 바디 </param>
		/// <param name="shape"> 도형 </param>
		/// <param name="line"> 직선 컨테이너 </param>
		void extractDebugHeightField(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>& line);

	private:
		std::unordered_map<unsigned int, PolygonMesh> mDebugPolygon;

		std::unordered_map<unsigned int, std::vector<unsigned int>> mDebugIndices;
		std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>> mDebugVertices;

		std::unordered_map<unsigned int, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>> mDebugHeightField;
	};

	const std::unordered_map<unsigned int, PolygonMesh>& DebugData::GetDebugPolygon()
	{
		return mDebugPolygon;
	}
	const std::unordered_map<unsigned int, std::vector<unsigned int>>& DebugData::GetDebugTriangleIndiecs()
	{
		return mDebugIndices;
	}
	const std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>>& DebugData::GetDebugTriangleVertices()
	{
		return mDebugVertices;
	}
	const std::unordered_map<unsigned int, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>>& DebugData::GetDebugHeightField()
	{
		return mDebugHeightField;
	}
}

