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
		bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType);
		bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType);
		bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType);
		bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType);
		bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType);
		bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType);
		bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType);
		bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType);

		/// <summary>
		/// ���̵� ���� ���� ������ �ٵ� �����մϴ�.
		/// </summary>
		virtual bool RemoveRigidBody(unsigned int id, physx::PxScene* scene);

		/// <summary>
		/// ���� ������ �ִ� ��� ������ �ٵ���� �����մϴ�.
		/// </summary>
		virtual bool RemoveAllRigidBody(physx::PxScene* scene);

	private:
		physx::PxPhysics* mPhysics;

		std::shared_ptr<PhysicsCookingMeshTool> mCookingMeshTool;
		std::map<unsigned int, std::shared_ptr<RigidBody>> mRigidBodys;
		std::vector<std::shared_ptr<RigidBody>> mUpcomingActors;
	};
}
