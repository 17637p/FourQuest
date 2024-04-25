#pragma once
#include "IFQPhysics.h"

#include <physx\PxPhysicsAPI.h>
#include <memory>

namespace fq::physics
{
	class Physics;
	class PhysicsRigidBodyManager;
	class PhysicsSimulationEventCallback;

	class FQPhysics : public IFQPhysics
	{
	public:
		FQPhysics();
		virtual ~FQPhysics();

		/// <summary>
		/// 물리 엔진을 세팅하여 물리 공간을 만듭니다.
		/// </summary>
		virtual bool Initialize(PhysicsEngineInfo& info) override;

		/// <summary>
		/// 물리 엔진의 업데이트를 통해 리지드 바디를 추가하고 시뮬레이션을 실행합니다.
		/// 시뮬레이션이 끝마쳤을 시에 충돌 이벤트가 콜백 함수를 통해 실행됩니다.
		/// </summary>
		virtual bool Update(float deltaTime) override;

		/// <summary>
		/// 물리 공간에 추가할 스태틱 바디 및 다이나믹 바디 생성합니다.
		/// </summary>
		virtual bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) override;

		/// <summary>
		/// 아이디 값을 받은 리지드 바디를 삭제합니다.
		/// </summary>
		virtual bool RemoveRigidBody(unsigned int id) override;

		/// <summary>
		/// 물리 공간에 있는 모든 리지드 바디들을 삭제합니다.
		/// </summary>
		virtual bool RemoveAllRigidBody() override;

	private:
		// 씬
		physx::PxScene* mScene;
		std::shared_ptr<Physics> mPhysics;
		std::shared_ptr<PhysicsRigidBodyManager> mRigidBodyManager;

		// 충돌 이벤트 클래스
		std::shared_ptr<PhysicsSimulationEventCallback> mMyEventCallback;

		// 충돌 매트릭스
		int* mCollisionMatrix;
	};
}

