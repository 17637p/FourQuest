#pragma once

#include "FQCommonPhysics.h"
#include <physx\PxPhysicsAPI.h>

#include <map>
#include <memory>
#include <set>

namespace fq::physics
{
	class RigidBody;
	class PhysicsCookingMeshTool;
	class StaticRigidBody;
	class DynamicRigidBody;

	class PhysicsRigidBodyManager
	{
	public:
		PhysicsRigidBodyManager();
		~PhysicsRigidBodyManager();

		/// <summary>
		/// ������ �ٵ� ���� �� �����ϴ� �Ŵ����� �����մϴ�.
		/// </summary>
		bool Initialize(physx::PxPhysics* physics);

		/// <summary>
		/// ������ ������ �ٵ���� �� ���� ���� ������ �����մϴ�.
		/// </summary>
		bool Update(physx::PxScene* scene, int* collisionMatrix);

		/// <summary>
		/// ����� ������ �� ���� ���� ������ Ŭ����� ������Ʈ
		/// </summary>
		bool FinalUpdate();

		/// <summary>
		/// ������ �ٵ���� �浹 ���� ������ ( ���̾� �ѹ�, �浹 ��Ʈ���� ) �� �����մϴ�.
		/// </summary>
		bool UpdateFilterData(std::shared_ptr<StaticRigidBody> body, int* collisionMatrix);
		bool UpdateFilterData(std::shared_ptr<DynamicRigidBody> body, int* collisionMatrix);

		/// <summary>
		/// ���� ������ �߰��� ����ƽ �ٵ� �� ���̳��� �ٵ� �����մϴ�.
		/// </summary>
		/// <param name="info"> ������ �ٵ��� ���� ���� �Դϴ�. </param>
		/// <param name="colliderType"> �ݶ��̴� Ÿ��(Ʈ����, �ݸ���) �Դϴ�. </param>
		bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);

		void GetRigidBodyMatrix(unsigned int id, DirectX::SimpleMath::Matrix& dxMatrix);

		/// <summary>
		/// ���̵� ������ �ش� ���̵��� ������ �ٵ𿡰� ������ Ʈ���������� �̵� ( �����̵� )
		/// </summary>
		bool SetRigidBodyMatrix(const unsigned int& id, const DirectX::SimpleMath::Matrix& worldTransform);

		/// <summary>
		/// ���̵� ������ �ش� ���̵��� ������ �ٵ𿡰� �ӵ� �߰�
		/// </summary>
		bool AddRigidBodyVelocity(const unsigned int& id, const DirectX::SimpleMath::Vector3& velocity);

		/// <summary>
		/// ���̵� ���� ���� ������ �ٵ� �����մϴ�.
		/// </summary>
		bool RemoveRigidBody(const unsigned int& id, physx::PxScene* scene);

		/// <summary>
		/// ���� ������ �ִ� ��� ������ �ٵ���� �����մϴ�.
		/// </summary>
		bool RemoveAllRigidBody(physx::PxScene* scene);

		/// <summary>
		/// �浹 ��Ʈ������ ������ �ȴٸ� ��� ������ �ٵ���� �浹 ��Ʈ������ ������Ʈ�մϴ�.
		/// </summary>
		/// <param name="collisionMatrix"></param>
		void UpdateCollisionMatrix(int* collisionMatrix);

		/// <summary>
		/// ���� ������ �ٵ���� ����� �����͸� �����մϴ�.
		/// </summary>
		void ExtractDebugData();

		inline const std::set<std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>>& GetDebugPolygon();
		inline std::unordered_map<unsigned int, std::shared_ptr<RigidBody>>& GetRigidBodies();

	private:
		physx::PxPhysics* mPhysics;

		std::shared_ptr<PhysicsCookingMeshTool> mCookingMeshTool;
		std::unordered_map<unsigned int, std::shared_ptr<RigidBody>> mRigidBodys;
		std::vector<std::shared_ptr<RigidBody>> mUpcomingActors;

		std::set<std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>> mDebugPolygon;
	};

	const std::set<std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>>& PhysicsRigidBodyManager::GetDebugPolygon()
	{
		return mDebugPolygon;
	}

	std::unordered_map<unsigned int, std::shared_ptr<RigidBody>>& PhysicsRigidBodyManager::GetRigidBodies()
	{
		return mRigidBodys;
	}
}
