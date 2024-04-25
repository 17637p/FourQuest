#pragma once

#include "FQCommonPhysics.h"

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
			/// 아이디 값을 받은 리지드 바디를 삭제합니다.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveRigidBody(unsigned int id) abstract;

			/// <summary>
			/// 물리 공간에 있는 모든 리지드 바디들을 삭제합니다.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveAllRigidBody() abstract;
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