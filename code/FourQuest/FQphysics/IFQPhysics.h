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
#pragma comment(lib, "PVDRuntime_64.lib")
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
			/// 콜백 함수를 등록합니다.
			/// </summary>
			/// <param name="func"> 콜리전 콜백 함수 등록 </param>
			virtual FQ_PHYSICS void SetCallBackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func) abstract;

			/// <summary>
			/// 물리 엔진의 수정할 정보를 전달 받습니다. ( 중력, 충돌 매트릭스 )
			/// </summary>
			/// <param name="info"> 물리 엔진 정보 </param>
			virtual FQ_PHYSICS void SetPhysicsInfo(PhysicsEngineInfo& info) abstract;

			/// <summary>
			/// 레이캐스트 : 원점, 방향, 거리값의 선을 쏴서 물리 공간의 오브젝트들을 충돌 검사
			/// </summary>
			virtual FQ_PHYSICS RayCastOutput RayCast(const RayCastInput& info, bool isStatic = false, bool isGpuScene = false) abstract;

#pragma region RigidBodyManager

			/// <summary>
			/// 물리 공간에 추가할 스태틱 바디 및 다이나믹 바디 생성
			/// </summary>
			virtual FQ_PHYSICS bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateStaticBody(const TriangleMeshColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateStaticBody(const HeightFieldColliderInfo& info, const EColliderType& colliderType) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const TriangleMeshColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) abstract;
			virtual FQ_PHYSICS bool CreateDynamicBody(const HeightFieldColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) abstract;

			/// <summary>
			/// 아이디를 받으면 해당 아이디의 리지드 바디를 반환
			/// </summary>
			virtual FQ_PHYSICS RigidBodyGetSetData GetRigidBodyData(const unsigned int& id) abstract;

			/// <summary>
			/// 아이디를 받으면 해당 아이디의 리지드 바디에게 지정한 트랜스폼으로 이동 ( 순간이동 )
			/// </summary>
			virtual FQ_PHYSICS bool SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData) abstract;

			/// <summary>
			/// 아이디 값을 받은 리지드 바디를 삭제합니다.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveRigidBody(const unsigned int& id) abstract;

			/// <summary>
			/// 물리 공간에 있는 모든 리지드 바디들을 삭제합니다.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveAllRigidBody() abstract;

			/// <summary>
			/// 컨벡스 메시의 디버그용 데이터를 전달 받습니다.
			/// </summary>
			/// <returns></returns>
			virtual FQ_PHYSICS const std::unordered_map<unsigned int, PolygonMesh>& GetDebugPolygon() abstract;

			/// <summary>
			/// Triangle Mesh의 디버그용 데이터를 전달 받습니다.
			/// </summary>
			/// <returns></returns>
			virtual FQ_PHYSICS const std::unordered_map<unsigned int, std::vector<unsigned int>>& GetDebugTriangleIndiecs() abstract;
			virtual FQ_PHYSICS const std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>>& GetDebugTriangleVertices() abstract;

			/// <summary>
			/// Height Field의 디버그용 데이터를 전달 받습니다.
			/// </summary>
			/// <returns></returns>
			virtual FQ_PHYSICS const std::unordered_map<unsigned int, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>>& GetDebugHeightField() abstract;

			/// <summary>
			/// 씬 체인지할 시에 모든 리지드 바디를 삭제하고 이벤트 콜백을 씬이 넘어가기 전에 처리합니다.
			/// </summary>
			/// <returns></returns>
			virtual FQ_PHYSICS bool ChangeScene() abstract;

#pragma endregion

#pragma region CharacterControllerManager

			/// <summary>
			/// 캐릭터 컨트롤러 생성 함수
			/// </summary>
			/// <param name="controllerInfo"> 캐릭터 컨트롤러 생성 데이터 </param>
			/// <param name="movementInfo"> 캐릭터 무브먼트 생성 데이터 </param>
			virtual FQ_PHYSICS bool CreateCCT(const CharacterControllerInfo& controllerInfo, const CharacterMovementInfo& movementInfo) abstract;

			/// <summary>
			/// 캐릭터 컨트롤러 삭제 함수
			/// </summary>
			/// <param name="id"> 삭제할 캐릭터 컨트롤러의 아이디 </param>
			virtual FQ_PHYSICS bool RemoveController(const unsigned int& id) abstract;

			/// <summary>
			/// 캐릭터 컨트롤러 삭제 함수
			/// </summary>
			virtual FQ_PHYSICS bool RemoveAllController() abstract;

			/// <summary>
			/// 특정 캐릭터 컨트롤러의 입력 값 추가 함수
			/// </summary>
			/// <param name="id"> 캐릭터 컨트롤러 아이디 </param>
			/// <param name="input"> 입력한 이동 방향 (ex. {1.f, 0.f, 0.f}) </param>
			virtual FQ_PHYSICS bool AddInputMove(const CharacterControllerInputInfo& info) abstract;

			/// <summary>
			/// 캐릭터 컨트롤러와 캐릭터 무브먼트의 데이터를 Get Set할 수 있습니다.
			/// </summary>
			virtual FQ_PHYSICS CharacterControllerGetSetData GetCharacterControllerData(const unsigned int& id) abstract;
			virtual FQ_PHYSICS CharacterMovementGetSetData GetCharacterMovementData(const unsigned int& id) abstract;
			virtual FQ_PHYSICS void SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData) abstract;
			virtual FQ_PHYSICS void SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData) abstract;

#pragma endregion

#pragma region CharacterPhysicsManager
			/// <summary>
			/// 캐릭터 파직스 (관절) 추가
			/// </summary>
			virtual FQ_PHYSICS bool CreateCharacterphysics(const ArticulationInfo& info) abstract;

			/// <summary>
			/// 캐릭터 파직스 삭제
			/// </summary>
			virtual FQ_PHYSICS bool RemoveArticulation(const unsigned int& id) abstract;

			/// <summary>
			/// 가지고 있는 관절 중, 링크 및 조인트 추가
			/// </summary>
			virtual FQ_PHYSICS bool AddArticulationLink(unsigned int id, LinkInfo& info, const DirectX::SimpleMath::Vector3& extent) abstract;
			virtual FQ_PHYSICS bool AddArticulationLink(unsigned int id, LinkInfo& info, const float& radius) abstract;
			virtual FQ_PHYSICS bool AddArticulationLink(unsigned int id, LinkInfo& info, const float& halfHeight, const float& radius) abstract;
			virtual FQ_PHYSICS bool AddArticulationLink(unsigned int id, LinkInfo& info) abstract;

			virtual FQ_PHYSICS ArticulationGetData GetArticulationData(const unsigned int& id) abstract;
			virtual FQ_PHYSICS void SetArticulationData(const unsigned int& id, const ArticulationSetData& articulationData) abstract;

			virtual FQ_PHYSICS unsigned int GetArticulationCount() abstract;
#pragma endregion

#pragma region PhysicsClothManager
			/// <summary>
			/// 천을 생성합니다.
			/// </summary>
			virtual FQ_PHYSICS bool CreateCloth(const Cloth::CreateClothData& info) abstract;

			/// <summary>
			/// 천 데이터를 얻습니다.
			/// </summary>
			virtual FQ_PHYSICS bool GetClothData(unsigned int id, Cloth::GetSetClothData& data) abstract;

			/// <summary>
			/// 천 데이터를 세팅합니다.
			/// </summary>
			virtual FQ_PHYSICS bool SetClothData(unsigned int id, const Cloth::GetSetClothData& data) abstract;

			/// <summary>
			/// 천을 삭제합니다.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveCloth(unsigned int id) abstract;
#pragma endregion

#pragma region ResourceManager
			virtual FQ_PHYSICS bool HasConvexMeshResource(const unsigned int& hash) abstract;
#pragma endregion

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