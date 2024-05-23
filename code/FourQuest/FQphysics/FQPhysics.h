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
	class PhysicsResourceManager;
	class PhysicsCharactorControllerManager;
	class PhysicsCharacterPhysicsManager;

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

#pragma region RigidBodyManager
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
		virtual RigidBodyGetSetData GetRigidBodyData(const unsigned int& id) override;

		/// <summary>
		/// 아이디를 받으면 해당 아이디의 리지드 바디에게 지정한 트랜스폼으로 이동 ( 순간이동 )
		/// </summary>
		virtual bool SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData) override;

		/// <summary>
		/// 아이디 값을 받은 리지드 바디를 삭제합니다.
		/// </summary>
		virtual bool RemoveRigidBody(const unsigned int& id) override;

		/// <summary>
		/// 물리 공간에 있는 모든 리지드 바디들을 삭제합니다.
		/// </summary>
		virtual bool RemoveAllRigidBody() override;

		/// <summary>
		/// 폴리곤의 디버그 데이터 
		/// </summary>
		const std::unordered_map<unsigned int, PolygonMesh>& GetDebugPolygon() override;
#pragma endregion

#pragma region CharacterControllerManager
		/// <summary>
		/// 캐릭터 컨트롤러 생성 함수
		/// </summary>
		/// <param name="controllerInfo"> 캐릭터 컨트롤러 생성 데이터 </param>
		/// <param name="movementInfo"> 캐릭터 무브먼트 생성 데이터 </param>
		virtual bool CreateCCT(const CharacterControllerInfo& controllerInfo, const CharacterMovementInfo& movementInfo) override;

		/// <summary>
		/// 캐릭터 컨트롤러 삭제 함수
		/// </summary>
		/// <param name="id"> 삭제할 캐릭터 컨트롤러의 아이디 </param>
		virtual bool RemoveController(const unsigned int& id) override;

		/// <summary>
		/// 특정 캐릭터 컨트롤러의 입력 값 추가 함수
		/// </summary>
		/// <param name="id"> 캐릭터 컨트롤러 아이디 </param>
		/// <param name="input"> 입력한 이동 방향 (ex. {1.f, 0.f, 0.f}) </param>
		virtual bool AddInputMove(const unsigned int& id, const DirectX::SimpleMath::Vector3& input) override;

		/// <summary>
		/// 캐릭터 컨트롤러와 캐릭터 무브먼트의 데이터를 Get Set합니다.
		/// </summary>
		virtual CharacterControllerGetSetData GetCharacterControllerData(const unsigned int& id) override;
		virtual CharacterMovementGetSetData GetCharacterMovementData(const unsigned int& id) override;
		virtual void SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData) override;
		virtual void SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData) override;
#pragma endregion

#pragma region CharacterPhysicsManager
		/// <summary>
		/// 캐릭터 파직스 (관절) 추가
		/// </summary>
		virtual bool CreateCharacterphysics(const CharacterPhysicsInfo& info) override;

		/// <summary>
		/// 가지고 있는 관절 중, 링크 및 조인트 추가
		/// </summary>
		virtual bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const DirectX::SimpleMath::Vector3& extent) override;
		virtual bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const float& radius) override;
		virtual bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const float& halfHeight, const float& radius) override;

		/// <summary>
		/// 물리 공간에 추가하여 CharacterPhysics를 시뮬레이션할 캐릭터 파직스
		/// </summary>
		virtual bool SimulationCharacter(unsigned int id) override;
#pragma endregion

		virtual bool HasConvexMeshResource(const unsigned int& hash) override;

		/// <summary>
		/// spdlog를 설정합니다
		/// </summary>
		virtual FQ_PHYSICS std::shared_ptr<spdlog::logger> SetUpLogger(std::vector<spdlog::sink_ptr> sinks) override;

	private:
		// 씬
		physx::PxScene* mScene;
		std::shared_ptr<Physics> mPhysics;
		std::shared_ptr<PhysicsResourceManager> mResourceManager;
		std::shared_ptr<PhysicsRigidBodyManager> mRigidBodyManager;
		std::shared_ptr<PhysicsCharactorControllerManager> mCCTManager;
		std::shared_ptr<PhysicsCharacterPhysicsManager> mCharacterPhysicsManager;

		// 충돌 이벤트 클래스
		std::shared_ptr<PhysicsSimulationEventCallback> mMyEventCallback;

		// 충돌 매트릭스
		int mCollisionMatrix[16];
	};
}

