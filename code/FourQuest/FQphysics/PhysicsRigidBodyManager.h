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

	using PolygonMesh = std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>;

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
		/// 디버그 데이터 등 물리 엔진 데이터 클리어용 업데이트
		/// </summary>
		bool FinalUpdate(physx::PxScene* scene);

		/// <summary>
		/// 물리 공간에 추가할 스태틱 바디 및 다이나믹 바디 생성합니다.
		/// </summary>
		/// <param name="info"> 리지드 바디의 도형 형태 입니다. </param>
		/// <param name="colliderType"> 콜라이더 타입(트리거, 콜리전) 입니다. </param>
		bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);
		bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, int* collisionMatrix);

		void GetRigidBodyData(unsigned int id, RigidBodyGetSetData& rigidBodyData);

		/// <summary>
		/// 아이디를 받으면 해당 아이디의 리지드 바디에게 지정한 트랜스폼으로 이동 ( 순간이동 )
		/// </summary>
		bool SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData);

		/// <summary>
		/// 아이디 값을 받은 리지드 바디를 삭제합니다.
		/// </summary>
		bool RemoveRigidBody(const unsigned int& id, physx::PxScene* scene);

		/// <summary>
		/// 물리 공간에 있는 모든 리지드 바디들을 삭제합니다.
		/// </summary>
		bool RemoveAllRigidBody(physx::PxScene* scene);

		/// <summary>
		/// 충돌 매트릭스가 수정이 된다면 모든 리지드 바디들의 충돌 매트릭스를 업데이트합니다.
		/// </summary>
		/// <param name="collisionMatrix"></param>
		void UpdateCollisionMatrix(int* collisionMatrix);

		/// <summary>
		/// 현재 리지드 바디들의 디버그 데이터를 추출합니다.
		/// </summary>
		void ExtractDebugData();

		/// <summary>
		/// 물리 공간의 리지드 바디들을 체크하여 리지드 바디가 없는 userData 삭제
		/// </summary>
		void UpdateUserData(physx::PxScene* scene);

		inline std::unordered_map<unsigned int, PolygonMesh>& GetDebugPolygon();
		inline std::unordered_map<unsigned int, std::shared_ptr<RigidBody>>& GetRigidBodyContainer();

	private:
		physx::PxPhysics* mPhysics;

		std::shared_ptr<PhysicsCookingMeshTool> mCookingMeshTool;
		std::unordered_map<unsigned int, std::shared_ptr<RigidBody>> mRigidBodyContainer;
		std::unordered_map<unsigned int, CollisionData*> mCollisionDataContainer;
		std::vector<std::shared_ptr<RigidBody>> mUpcomingActors;

		std::unordered_map<unsigned int, PolygonMesh> mDebugPolygon;
	};

	std::unordered_map<unsigned int, PolygonMesh>& PhysicsRigidBodyManager::GetDebugPolygon()
	{
		return mDebugPolygon;
	}

	std::unordered_map<unsigned int, std::shared_ptr<RigidBody>>& PhysicsRigidBodyManager::GetRigidBodyContainer()
	{
		return mRigidBodyContainer;
	}
}
