#pragma once
#pragma once
#include "IFQPhysics.h"
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
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
	class PhysicsCollisionDataManager;
	class PhysicsClothManager;


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

		/// <summary>
		/// 씬 체인지할 시에 모든 리지드 바디를 삭제하고 이벤트 콜백을 씬이 넘어가기 전에 처리합니다.
		/// </summary>
		/// <returns></returns>
		virtual bool ChangeScene() override;

		/// <summary>
		/// 레이캐스트 : 원점, 방향, 거리값의 선을 쏴서 물리 공간의 오브젝트들을 충돌 검사
		/// </summary>
		virtual RayCastOutput RayCast(const RayCastInput& info, bool isStatic = false, bool isGPUScene = false) override;

#pragma region RigidBodyManager
		/// <summary>
		/// 물리 공간에 추가할 스태틱 바디 및 다이나믹 바디 생성합니다.
		/// </summary>
		virtual bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const TriangleMeshColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const HeightFieldColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) override;
		virtual bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) override;
		virtual bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) override;
		virtual bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) override;
		virtual bool CreateDynamicBody(const TriangleMeshColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) override;
		virtual bool CreateDynamicBody(const HeightFieldColliderInfo& info, const EColliderType& colliderType, bool isKinematic = false) override;

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

		/// <summary>
		/// Triangle Mesh의 디버그용 데이터를 전달 받습니다.
		/// </summary>
		/// <returns></returns>
		const std::unordered_map<unsigned int, std::vector<unsigned int>>& GetDebugTriangleIndiecs() override;
		const std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>>& GetDebugTriangleVertices() override;

		/// <summary>
		/// Height Field의 디버그용 데이터를 전달 받습니다.
		/// </summary>
		/// <returns></returns>
		const std::unordered_map<unsigned int, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>>& GetDebugHeightField() override;
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
		/// 캐릭터 컨트롤러 삭제 함수
		/// </summary>
		virtual bool RemoveAllController() override;

		/// <summary>
		/// 특정 캐릭터 컨트롤러의 입력 값 추가 함수
		/// </summary>
		/// <param name="id"> 캐릭터 컨트롤러 아이디 </param>
		/// <param name="input"> 입력한 이동 방향 (ex. {1.f, 0.f, 0.f}) </param>
		virtual bool AddInputMove(const CharacterControllerInputInfo& info) override;

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
		virtual bool CreateCharacterphysics(const ArticulationInfo& info) override;

		/// <summary>
		/// 캐릭터 파직스 삭제
		/// </summary>
		virtual bool RemoveArticulation(const unsigned int& id) override;

		/// <summary>
		/// 캐릭터 파직스 모두 삭제
		/// </summary>
		virtual bool RemoveAllArticulation() override;

		/// <summary>
		/// 가지고 있는 관절 중, 링크 및 조인트 추가
		/// </summary>
		virtual bool AddArticulationLink(unsigned int id, LinkInfo& info, const DirectX::SimpleMath::Vector3& extent) override;
		virtual bool AddArticulationLink(unsigned int id, LinkInfo& info, const float& radius) override;
		virtual bool AddArticulationLink(unsigned int id, LinkInfo& info, const float& halfHeight, const float& radius) override;
		virtual bool AddArticulationLink(unsigned int id, LinkInfo& info) override;

		/// <summary>
		/// 관절 (Articulation) Get/Set 데이터 입니다.
		/// </summary>
		virtual ArticulationGetData GetArticulationData(const unsigned int& id) override;
		virtual void SetArticulationData(const unsigned int& id, const ArticulationSetData& articulationData) override;

		/// <summary>
		/// 생성된 관절 갯수를 반환합니다.
		/// </summary>
		virtual unsigned int GetArticulationCount() override;
#pragma endregion

#pragma region PhysicsClothManager
		/// <summary>
		/// 천을 생성합니다.
		/// </summary>
		virtual bool CreateCloth(const Cloth::CreateClothData& info, bool isSkinnedMesh) override;

		/// <summary>
		/// 천 데이터를 얻습니다.
		/// </summary>
		virtual bool GetClothData(unsigned int id, Cloth::GetSetClothData& data) override;

		/// <summary>
		/// 천 데이터를 세팅합니다.
		/// </summary>
		virtual bool SetClothData(unsigned int id, const Cloth::GetSetClothData& data) override;

		/// <summary>
		/// 천을 삭제합니다.
		/// </summary>
		virtual bool RemoveCloth(unsigned int id) override;

		/// <summary>
		/// 모든 천을 삭제합니다.
		/// </summary>
		virtual bool RemoveAllCloth() override;

		/// <summary>
		/// 천의 버텍스를 반환합니다.
		/// </summary>
		virtual const std::vector<DirectX::SimpleMath::Vector4>& GetClothVertex(unsigned int id) override;

		/// <summary>
		/// 천의 인덱스를 반환합니다.
		/// </summary>
		virtual const std::vector<unsigned int>& GetClothIndices(unsigned int id) override;
#pragma endregion

		virtual bool HasConvexMeshResource(const unsigned int& hash) override;

		/// <summary>
		/// spdlog를 설정합니다
		/// </summary>
		virtual FQ_PHYSICS std::shared_ptr<spdlog::logger> SetUpLogger(std::vector<spdlog::sink_ptr> sinks) override;

		/// <summary>
		/// 삭제 예정인 액터 삭제하는 함수입니다.
		/// </summary>
		void RemoveActors();


	private:
		// 씬
		physx::PxScene* mScene;
		physx::PxScene* mGpuScene;
		physx::PxCudaContextManager* mCudaContextManager;

		std::shared_ptr<Physics> mPhysics;
		std::shared_ptr<PhysicsResourceManager> mResourceManager;
		std::shared_ptr<PhysicsRigidBodyManager> mRigidBodyManager;
		std::shared_ptr<PhysicsCharactorControllerManager> mCCTManager;
		std::shared_ptr<PhysicsCharacterPhysicsManager> mCharacterPhysicsManager;
		std::shared_ptr<PhysicsClothManager> mClothManager;
		std::shared_ptr<PhysicsCollisionDataManager> mCollisionDataManager;

		// 충돌 이벤트 클래스
		std::shared_ptr<PhysicsSimulationEventCallback> mMyEventCallback;

		// 충돌 매트릭스
		int mCollisionMatrix[COLLISION_MATRIX_SIZE];
		bool mbIsSimulating;
		bool mbIsHalfFrameUpdate;
		int mGpuSceneWaitUpdateCount;

		std::vector<physx::PxActor*> mActorsToRemove;
	};
}

