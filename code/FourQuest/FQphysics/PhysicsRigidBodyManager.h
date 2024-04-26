#pragma once

#include "FQCommonPhysics.h"
#include <physx\PxPhysicsAPI.h>

#include <map>
#include <memory>

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
		/// ������ �ٵ���� �浹 ���� ������ ( ���̾� �ѹ�, �浹 ��Ʈ���� ) �� �����մϴ�.
		/// </summary>
		bool UpdateFilterData(std::shared_ptr<StaticRigidBody> body, int* collisionMatrix);
		bool UpdateFilterData(std::shared_ptr<DynamicRigidBody> body, int* collisionMatrix);

		/// <summary>
		/// ���� ������ �߰��� ����ƽ �ٵ� �� ���̳��� �ٵ� �����մϴ�.
		/// </summary>
		/// <param name="info"> ������ �ٵ��� ���� ���� �Դϴ�. </param>
		/// <param name="colliderType"> �ݶ��̴� Ÿ��(Ʈ����, �ݸ���) �Դϴ�. </param>
		bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType);
		bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType);
		bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType);
		bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType);
		bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType);
		bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType);
		bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType);
		bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType);

		DirectX::SimpleMath::Matrix GetRigidBodyMatrix(unsigned int id);

		/// <summary>
		/// ���̵� ���� ���� ������ �ٵ� �����մϴ�.
		/// </summary>
		bool RemoveRigidBody(unsigned int id, physx::PxScene* scene);

		/// <summary>
		/// ���� ������ �ִ� ��� ������ �ٵ���� �����մϴ�.
		/// </summary>
		bool RemoveAllRigidBody(physx::PxScene* scene);

		/// <summary>
		/// �浹 ��Ʈ������ ������ �ȴٸ� ��� ������ �ٵ���� �浹 ��Ʈ������ ������Ʈ�մϴ�.
		/// </summary>
		/// <param name="collisionMatrix"></param>
		void UpdateCollisionMatrix(int* collisionMatrix);

	private:
		physx::PxPhysics* mPhysics;

		std::shared_ptr<PhysicsCookingMeshTool> mCookingMeshTool;
		std::map<unsigned int, std::shared_ptr<RigidBody>> mRigidBodys;
		std::vector<std::shared_ptr<RigidBody>> mUpcomingActors;
	};
}
