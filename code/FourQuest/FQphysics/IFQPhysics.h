#pragma once

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

			/// Framework
			virtual FQ_PHYSICS bool Initialize() abstract;
			virtual FQ_PHYSICS bool Update(float deltaTime) abstract;

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