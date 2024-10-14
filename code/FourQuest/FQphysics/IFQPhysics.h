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
			/// ���� ������ �����Ͽ� ���� ������ ����ϴ�.
			/// </summary>
			virtual FQ_PHYSICS bool Initialize(PhysicsEngineInfo& info) abstract;

			/// <summary>
			/// ���� ������ ������Ʈ�� ���� ������ �ٵ� �߰��ϰ� �ùķ��̼��� �����մϴ�.
			/// �ùķ��̼��� �������� �ÿ� �浹 �̺�Ʈ�� �ݹ� �Լ��� ���� ����˴ϴ�.
			/// </summary>
			virtual FQ_PHYSICS bool Update(float deltaTime) abstract;

			/// <summary>
			/// ����� ������ �� ���� ���� ������ Ŭ����� ������Ʈ
			/// </summary>
			virtual FQ_PHYSICS bool FinalUpdate() abstract;

			/// <summary>
			/// �ݹ� �Լ��� ����մϴ�.
			/// </summary>
			/// <param name="func"> �ݸ��� �ݹ� �Լ� ��� </param>
			virtual FQ_PHYSICS void SetCallBackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func) abstract;

			/// <summary>
			/// ���� ������ ������ ������ ���� �޽��ϴ�. ( �߷�, �浹 ��Ʈ���� )
			/// </summary>
			/// <param name="info"> ���� ���� ���� </param>
			virtual FQ_PHYSICS void SetPhysicsInfo(PhysicsEngineInfo& info) abstract;

			/// <summary>
			/// ����ĳ��Ʈ : ����, ����, �Ÿ����� ���� ���� ���� ������ ������Ʈ���� �浹 �˻�
			/// </summary>
			virtual FQ_PHYSICS RayCastOutput RayCast(const RayCastInput& info, bool isStatic = false, bool isGpuScene = false) abstract;

#pragma region RigidBodyManager

			/// <summary>
			/// ���� ������ �߰��� ����ƽ �ٵ� �� ���̳��� �ٵ� ����
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
			/// ���̵� ������ �ش� ���̵��� ������ �ٵ� ��ȯ
			/// </summary>
			virtual FQ_PHYSICS RigidBodyGetSetData GetRigidBodyData(const unsigned int& id) abstract;

			/// <summary>
			/// ���̵� ������ �ش� ���̵��� ������ �ٵ𿡰� ������ Ʈ���������� �̵� ( �����̵� )
			/// </summary>
			virtual FQ_PHYSICS bool SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData) abstract;

			/// <summary>
			/// ���̵� ���� ���� ������ �ٵ� �����մϴ�.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveRigidBody(const unsigned int& id) abstract;

			/// <summary>
			/// ���� ������ �ִ� ��� ������ �ٵ���� �����մϴ�.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveAllRigidBody() abstract;

			/// <summary>
			/// ������ �޽��� ����׿� �����͸� ���� �޽��ϴ�.
			/// </summary>
			/// <returns></returns>
			virtual FQ_PHYSICS const std::unordered_map<unsigned int, PolygonMesh>& GetDebugPolygon() abstract;

			/// <summary>
			/// Triangle Mesh�� ����׿� �����͸� ���� �޽��ϴ�.
			/// </summary>
			/// <returns></returns>
			virtual FQ_PHYSICS const std::unordered_map<unsigned int, std::vector<unsigned int>>& GetDebugTriangleIndiecs() abstract;
			virtual FQ_PHYSICS const std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>>& GetDebugTriangleVertices() abstract;

			/// <summary>
			/// Height Field�� ����׿� �����͸� ���� �޽��ϴ�.
			/// </summary>
			/// <returns></returns>
			virtual FQ_PHYSICS const std::unordered_map<unsigned int, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>>& GetDebugHeightField() abstract;

			/// <summary>
			/// �� ü������ �ÿ� ��� ������ �ٵ� �����ϰ� �̺�Ʈ �ݹ��� ���� �Ѿ�� ���� ó���մϴ�.
			/// </summary>
			/// <returns></returns>
			virtual FQ_PHYSICS bool ChangeScene() abstract;

#pragma endregion

#pragma region CharacterControllerManager

			/// <summary>
			/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
			/// </summary>
			/// <param name="controllerInfo"> ĳ���� ��Ʈ�ѷ� ���� ������ </param>
			/// <param name="movementInfo"> ĳ���� �����Ʈ ���� ������ </param>
			virtual FQ_PHYSICS bool CreateCCT(const CharacterControllerInfo& controllerInfo, const CharacterMovementInfo& movementInfo) abstract;

			/// <summary>
			/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
			/// </summary>
			/// <param name="id"> ������ ĳ���� ��Ʈ�ѷ��� ���̵� </param>
			virtual FQ_PHYSICS bool RemoveController(const unsigned int& id) abstract;

			/// <summary>
			/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
			/// </summary>
			virtual FQ_PHYSICS bool RemoveAllController() abstract;

			/// <summary>
			/// Ư�� ĳ���� ��Ʈ�ѷ��� �Է� �� �߰� �Լ�
			/// </summary>
			/// <param name="id"> ĳ���� ��Ʈ�ѷ� ���̵� </param>
			/// <param name="input"> �Է��� �̵� ���� (ex. {1.f, 0.f, 0.f}) </param>
			virtual FQ_PHYSICS bool AddInputMove(const CharacterControllerInputInfo& info) abstract;

			/// <summary>
			/// ĳ���� ��Ʈ�ѷ��� ĳ���� �����Ʈ�� �����͸� Get Set�� �� �ֽ��ϴ�.
			/// </summary>
			virtual FQ_PHYSICS CharacterControllerGetSetData GetCharacterControllerData(const unsigned int& id) abstract;
			virtual FQ_PHYSICS CharacterMovementGetSetData GetCharacterMovementData(const unsigned int& id) abstract;
			virtual FQ_PHYSICS void SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData) abstract;
			virtual FQ_PHYSICS void SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData) abstract;

#pragma endregion

#pragma region CharacterPhysicsManager
			/// <summary>
			/// ĳ���� ������ (����) �߰�
			/// </summary>
			virtual FQ_PHYSICS bool CreateCharacterphysics(const ArticulationInfo& info) abstract;

			/// <summary>
			/// ĳ���� ������ ����
			/// </summary>
			virtual FQ_PHYSICS bool RemoveArticulation(const unsigned int& id) abstract;

			/// <summary>
			/// ������ �ִ� ���� ��, ��ũ �� ����Ʈ �߰�
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
			/// õ�� �����մϴ�.
			/// </summary>
			virtual FQ_PHYSICS bool CreateCloth(const Cloth::CreateClothData& info) abstract;

			/// <summary>
			/// õ �����͸� ����ϴ�.
			/// </summary>
			virtual FQ_PHYSICS bool GetClothData(unsigned int id, Cloth::GetSetClothData& data) abstract;

			/// <summary>
			/// õ �����͸� �����մϴ�.
			/// </summary>
			virtual FQ_PHYSICS bool SetClothData(unsigned int id, const Cloth::GetSetClothData& data) abstract;

			/// <summary>
			/// õ�� �����մϴ�.
			/// </summary>
			virtual FQ_PHYSICS bool RemoveCloth(unsigned int id) abstract;
#pragma endregion

#pragma region ResourceManager
			virtual FQ_PHYSICS bool HasConvexMeshResource(const unsigned int& hash) abstract;
#pragma endregion

			/// <summary>
			/// spdlog�� �����մϴ�
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