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
		bool FinalUpdate();

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
		/// 아이디를 받으면 해당 아이디의 리지드 바디에게 지정한 트랜스폼으로 이동 ( 순간이동 )
		/// </summary>
		bool SetRigidBodyMatrix(const unsigned int& id, const DirectX::SimpleMath::Matrix& worldTransform);

		/// <summary>
		/// 아이디를 받으면 해당 아이디의 리지드 바디에게 속도 추가
		/// </summary>
		bool AddRigidBodyVelocity(const unsigned int& id, const DirectX::SimpleMath::Vector3& velocity);

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

		inline const std::set<std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>>& GetDebugPolygon();

	private:
		physx::PxPhysics* mPhysics;

		std::shared_ptr<PhysicsCookingMeshTool> mCookingMeshTool;
		std::map<unsigned int, std::shared_ptr<RigidBody>> mRigidBodys;
		std::vector<std::shared_ptr<RigidBody>> mUpcomingActors;

		std::set<std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>> mDebugPolygon;
	};

	const std::set<std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>>& PhysicsRigidBodyManager::GetDebugPolygon()
	{
		return mDebugPolygon;
	}
}
