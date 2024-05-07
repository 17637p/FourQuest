#pragma once

#include "../FQCommon/FQCommonPhysics.h"

#include <memory>
#include <set>
#include <spdlog/spdlog.h>
#include <spdlog/logger.h>

#pragma comment(lib, "LowLevel_static_64.lib")
#pragma comment(lib, "LowLevelAABB_static_64.lib")
#pragma comment(lib, "LowLevelDynamics_static_64.lib")
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXTask_static_64.lib")
#pragma comment(lib, "PhysXVehicle_static_64.lib")
#pragma comment(lib, "PhysXVehicle2_static_64.lib")
//#pragma comment(lib, "PVDRuntime_64.lib")
#pragma comment(lib, "SceneQuery_static_64.lib")
#pragma comment(lib, "SimulationController_static_64.lib")

#ifdef FQ_PHYSICS_EXPORT
#define FQ_PHYSICS __declspec(dllexport)
#else
#define FQ_PHYSICS __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	namespace fq::physics
	{
		using PolygonMesh = std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>;

		class IFQPhysics
		{
		public:
			virtual ~IFQPhysics() {};

			/// <summary>
			/// 물리 엔진을 세팅하여 물리 공간을 만듭니다.
			/// </summary>
			virtual FQ_PHYSICS bool Initialize(PhysicsEngineInfo& info) abstract;

			/// <summary>
			/// 물리 엔진의 업데이트를 통해 리지드 바디를 추가하고 시뮬레이션을 실행합니다.
			/// 시뮬레이션이 끝마쳤을 시에 충돌 이벤트가 콜백 함수를 통해 실행됩니다.
			/// </summary>
			virtual FQ_PHYSICS bool Update(float deltaTime) abstract;

			/// <summary>
			/// 디버그 데이터 등 물리 엔진 데이터 클리어용 업데이트
			/// </summary>
			virtual FQ_PHYSICS bool FinalUpdate() abstract;

			/// <summary>
			/// 물리 공간에 추가할 스태틱 바디 및 다이나믹 바디 생성
			/// </summary>
			virtual FQ_PHYSICS bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) abstract;

			/// <summary>
			/// 아이디를 받으면 해당 아이디의 리지드 바디를 반환
			/// </summary>
			virtual FQ_PHYSICS DirectX::SimpleMath::Matrix GetRigidBodyMatrix(const unsigned int& id) abstract;

			/// <summary>
			/// 아이디를 받으면 해당 아이디의 리지드 바디에게 지정한 트랜스폼으로 이동 ( 순간이동 )
			/// </summary>
			virtual FQ_PHYSICS bool SetRigidBodyMatrix(const unsigned int& id, const DirectX::SimpleMath::Matrix& worldTransform) abstract;

			/// <summary>
			/// 아이디를 받으면 해당 아이디의 리지드 바디에게 속도 추가
			/// </summary>
			virtual FQ_PHYSICS bool AddRigidBodyVelocity(const unsigned int& id, const DirectX::SimpleMath::Vector3& velocity) abstract;

			/// <summary>
			/// 아이디 값을 받은 리지드 바디를 삭제합니다.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveRigidBody(const unsigned int& id) abstract;

			/// <summary>
			/// 물리 공간에 있는 모든 리지드 바디들을 삭제합니다.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveAllRigidBody() abstract;

			/// <summary>
			/// 콜백 함수를 등록합니다.
			/// </summary>
			/// <param name="func"> 콜리전 콜백 함수 등록 </param>
			virtual FQ_PHYSICS void SetCallBackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func) abstract;

			/// <summary>
			/// 물리 엔진의 수정할 정보를 전달 받습니다. ( 중력, 충돌 매트릭스 )
			/// </summary>
			/// <param name="info"> 물리 엔진 정보 </param>
			virtual FQ_PHYSICS void SetPhysicsInfo(PhysicsEngineInfo& info) abstract;

			virtual FQ_PHYSICS const std::unordered_map<unsigned int, PolygonMesh>& GetDebugPolygon() abstract;

			/// <summary>
			/// spdlog를 설정합니다
			/// </summary>
			virtual FQ_PHYSICS std::shared_ptr<spdlog::logger> SetUpLogger(std::vector<spdlog::sink_ptr> sinks) abstract;
		};

		class EngineExporter
		{
		public:
			FQ_PHYSICS IFQPhysics* GetEngine();
			FQ_PHYSICS void DeleteEngine(IFQPhysics* engine);
		};
	}

#ifdef __cplusplus
}
#endif