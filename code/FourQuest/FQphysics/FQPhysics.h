#pragma once
#include "IFQPhysics.h"
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <physx\PxPhysicsAPI.h>
#include <memory>
#include <set>

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
		/// 디버그 데이터 등 물리 엔진 데이터 클리어용 업데이트
		/// </summary>
		virtual bool FinalUpdate() override;

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
		/// 아이디를 받으면 해당 아이디의 리지드 바디를 반환
		/// </summary>
		virtual DirectX::SimpleMath::Matrix GetRigidBodyMatrix(const unsigned int& id) override;

		/// <summary>
		/// 아이디를 받으면 해당 아이디의 리지드 바디에게 지정한 트랜스폼으로 이동 ( 순간이동 )
		/// </summary>
		virtual bool SetRigidBodyMatrix(const unsigned int& id, const DirectX::SimpleMath::Matrix& worldTransform) override;

		/// <summary>
		/// 아이디를 받으면 해당 아이디의 리지드 바디에게 속도 추가
		/// </summary>
		virtual bool AddRigidBodyVelocity(const unsigned int& id, const DirectX::SimpleMath::Vector3& velocity) override;

		/// <summary>
		/// 아이디 값을 받은 리지드 바디를 삭제합니다.
		/// </summary>
		virtual bool RemoveRigidBody(const unsigned int& id) override;

		/// <summary>
		/// 물리 공간에 있는 모든 리지드 바디들을 삭제합니다.
		/// </summary>
		virtual bool RemoveAllRigidBody() override;

		/// <summary>
		/// 콜백 함수 등록
		/// </summary>
		/// <param name="func"> 콜리전 데이터 구조체 </param>
		/// <param name="eventType"> 콜리전 이벤트 타입 </param>
		virtual void SetCallBackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func) override;

		/// <summary>
		/// 물리 엔진의 수정할 정보를 전달 받습니다. ( 중력, 충돌 매트릭스 )
		/// </summary>
		/// <param name="info"> 물리 엔진 정보 </param>
		virtual void SetPhysicsInfo(PhysicsEngineInfo& info) override;

		/// <summary>
		/// 폴리곤의 디버그 데이터 
		/// </summary>
		const std::unordered_map<unsigned int, PolygonMesh>& GetDebugPolygon() override;

		/// <summary>
		/// spdlog를 설정합니다
		/// </summary>
		virtual FQ_PHYSICS std::shared_ptr<spdlog::logger> SetUpLogger(std::vector<spdlog::sink_ptr> sinks) override;

	private:
		// 씬
		physx::PxScene* mScene;
		std::shared_ptr<Physics> mPhysics;
		std::shared_ptr<PhysicsRigidBodyManager> mRigidBodyManager;

		// 충돌 이벤트 클래스
		std::shared_ptr<PhysicsSimulationEventCallback> mMyEventCallback;

		// 충돌 매트릭스
		int mCollisionMatrix[16];
	};
}

