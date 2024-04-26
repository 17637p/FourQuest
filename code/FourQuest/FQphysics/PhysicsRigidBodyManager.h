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
		/// 리지드 바디를 생성 및 관리하는 매니저를 세팅합니다.
		/// </summary>
		bool Initialize(physx::PxPhysics* physics);

		/// <summary>
		/// 생성된 리지드 바디들을 한 번에 물리 공간에 생성합니다.
		/// </summary>
		bool Update(physx::PxScene* scene, int* collisionMatrix);

		/// <summary>
		/// 리지드 바디들의 충돌 필터 데이터 ( 레이어 넘버, 충돌 매트릭스 ) 를 세팅합니다.
		/// </summary>
		bool UpdateFilterData(std::shared_ptr<StaticRigidBody> body, int* collisionMatrix);
		bool UpdateFilterData(std::shared_ptr<DynamicRigidBody> body, int* collisionMatrix);

		/// <summary>
		/// 물리 공간에 추가할 스태틱 바디 및 다이나믹 바디 생성합니다.
		/// </summary>
		/// <param name="info"> 리지드 바디의 도형 형태 입니다. </param>
		/// <param name="colliderType"> 콜라이더 타입(트리거, 콜리전) 입니다. </param>
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
		/// 아이디 값을 받은 리지드 바디를 삭제합니다.
		/// </summary>
		bool RemoveRigidBody(unsigned int id, physx::PxScene* scene);

		/// <summary>
		/// 물리 공간에 있는 모든 리지드 바디들을 삭제합니다.
		/// </summary>
		bool RemoveAllRigidBody(physx::PxScene* scene);

		/// <summary>
		/// 충돌 매트릭스가 수정이 된다면 모든 리지드 바디들의 충돌 매트릭스를 업데이트합니다.
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
