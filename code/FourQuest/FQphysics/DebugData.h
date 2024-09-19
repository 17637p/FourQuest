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
		/// ConvexMesh ������ ���� RigidBody ����� ������ ����
		/// </summary>
		/// <param name="_body"> ������ �ٵ� </param>
		/// <param name="_shape"> ���� </param>
		void extractDebugConvexMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<std::vector<DirectX::SimpleMath::Vector3>>& debugPolygon);

		/// <summary>
		/// TriangleMesh ������ ���� RigidBody ����� ������ ����
		/// </summary>
		/// <param name="body"> ������ �ٵ� </param>
		/// <param name="shape"> ���� </param>
		/// <param name="indices"> �ε��� �����̳� </param>
		/// <param name="vertices"> ���ؽ� �����̳� </param>
		void extractDebugTriangleMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<unsigned int>& indices, std::vector<DirectX::SimpleMath::Vector3>& vertices);

		/// <summary>
		/// �ͷ����� ���� ����Ʈ �ʵ��� ����� ������ ����
		/// </summary>
		/// <param name="body"> ������ �ٵ� </param>
		/// <param name="shape"> ���� </param>
		/// <param name="line"> ���� �����̳� </param>
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

