#include "FQPhysics.h"

#include "Physics.h"
#include "PhysicsRigidBodyManager.h"
#include "PhysicsCharactorControllerManager.h"
#include "PhysicsCharacterPhysicsManager.h"
#include "PhysicsResourceManager.h"
#include "PhysicsCollisionDataManager.h"
#include "PhysicsClothManager.h"
#include "PhysicsSimulationEventCallback.h"
#include "RaycastQueryFileter.h"

#include "ConvexMeshResource.h"
#include "EngineDataConverter.h"

namespace fq::physics
{
	/// <summary>
	/// 충돌 콜백 함수
	/// <summary>
	physx::PxFilterFlags CustomSimulationFilterShader(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		/// <summary>
		/// 쌍에 대해 CCD를 활성화하고 초기 및 CCD 연락처에 대한 연락처 보고서를 요청합니다.
		/// 또한 접점별 정보를 제공하고 행위자에게 정보를 제공합니다
		/// 접촉할 때 포즈를 취합니다.
		/// <summary>

		// 필터 셰이더 로직 ( 트리거 )
		if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			if ((((1 << filterData0.word0) & filterData1.word1) > 0) && (((1 << filterData1.word0) & filterData0.word1) > 0))
			{
				pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT
					| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
					| physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
				return physx::PxFilterFlag::eDEFAULT;
			}
		}

		// 필터 데이터 충돌 체크 ( 시뮬레이션 )
		if ((((1 << filterData0.word0) & filterData1.word1) > 0) && (((1 << filterData1.word0) & filterData0.word1) > 0))
		{
			pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT
				| physx::PxPairFlag::eDETECT_CCD_CONTACT
				| physx::PxPairFlag::eNOTIFY_TOUCH_CCD
				| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
				| physx::PxPairFlag::eNOTIFY_TOUCH_LOST
				| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS
				| physx::PxPairFlag::eCONTACT_EVENT_POSE
				| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
			return physx::PxFilterFlag::eDEFAULT;
		}
		else
		{
			pairFlags &= ~physx::PxPairFlag::eCONTACT_DEFAULT; // 충돌 행동 비허용
			return physx::PxFilterFlag::eSUPPRESS;
		}
	}

	physx::PxFilterFlags CustomGpuSimulationFilterShader(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		/// <summary>
		/// 쌍에 대해 CCD를 활성화하고 초기 및 CCD 연락처에 대한 연락처 보고서를 요청합니다.
		/// 또한 접점별 정보를 제공하고 행위자에게 정보를 제공합니다
		/// 접촉할 때 포즈를 취합니다.
		/// <summary>

		// 필터 셰이더 로직 ( 트리거 )
		if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT
				| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
				| physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
			return physx::PxFilterFlag::eDEFAULT;
		}

		// 필터 데이터 충돌 체크 ( 시뮬레이션 )
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT
			| physx::PxPairFlag::eDETECT_CCD_CONTACT
			| physx::PxPairFlag::eNOTIFY_TOUCH_CCD
			| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
			| physx::PxPairFlag::eNOTIFY_TOUCH_LOST
			| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS
			| physx::PxPairFlag::eCONTACT_EVENT_POSE
			| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		return physx::PxFilterFlag::eDEFAULT;
	}

	FQPhysics::FQPhysics()
		: mPhysics(std::make_shared<Physics>())
		, mRigidBodyManager(std::make_shared<PhysicsRigidBodyManager>())
		, mCCTManager(std::make_shared<PhysicsCharactorControllerManager>())
		, mResourceManager(std::make_shared<PhysicsResourceManager>())
		, mCharacterPhysicsManager(std::make_shared<PhysicsCharacterPhysicsManager>())
		, mClothManager(std::make_shared<PhysicsClothManager>())
		, mCollisionDataManager(std::make_shared<PhysicsCollisionDataManager>())
		, mMyEventCallback(std::make_shared<PhysicsSimulationEventCallback>())
		, mScene(nullptr)
		, mGpuScene(nullptr)
		, mCudaContextManager(nullptr)
		, mCollisionMatrix{}
	{
	}

	FQPhysics::~FQPhysics()
	{
		mCCTManager = nullptr;
		mRigidBodyManager = nullptr;
		PX_RELEASE(mScene);
		PX_RELEASE(mGpuScene);
		PX_RELEASE(mCudaContextManager);
	}

	bool FQPhysics::Initialize(PhysicsEngineInfo& info)
	{
		mPhysics->Initialize();

		physx::PxPhysics* physics = mPhysics->GetPhysics();

		// PhysX 시뮬레이션을 위한 Scene을 설정합니다.
		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale()); // Scene을 생성할 때 물리적인 허용 오차 스케일을 설정합니다.

		if (PxGetSuggestedCudaDeviceOrdinal(mPhysics->GetFoundation()->getErrorCallback()) >= 0)
		{
			// initialize CUDA
			physx::PxCudaContextManagerDesc cudaContextManagerDesc;
			mCudaContextManager = PxCreateCudaContextManager(*mPhysics->GetFoundation(), cudaContextManagerDesc, PxGetProfilerCallback());
			if (mCudaContextManager && !mCudaContextManager->contextIsValid())
			{
				mCudaContextManager->release();
				mCudaContextManager = NULL;
			}
		}
		if (mCudaContextManager == NULL)
		{
			PxGetFoundation().error(physx::PxErrorCode::eINVALID_OPERATION, PX_FL, "Failed to initialize CUDA!\n");
		}

		// 중력을 설정합니다.
		sceneDesc.gravity.x = info.gravity.x;
		sceneDesc.gravity.y = info.gravity.y;
		sceneDesc.gravity.z = -info.gravity.z;
		memcpy(mCollisionMatrix, info.collisionMatrix, sizeof(int) * COLLISION_MATRIX_SIZE);

		// Scene 설명자에 CPU 디스패처와 필터 셰이더를 설정합니다.
		sceneDesc.cpuDispatcher = mPhysics->GetDispatcher();
		sceneDesc.filterShader = CustomSimulationFilterShader;
		sceneDesc.simulationEventCallback = mMyEventCallback.get();
		sceneDesc.staticStructure = physx::PxPruningStructureType::eDYNAMIC_AABB_TREE;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;
		sceneDesc.broadPhaseType = physx::PxBroadPhaseType::ePABP;
		sceneDesc.solverType = physx::PxSolverType::ePGS;

		// PhysX Physics에서 PhysX의 Scene을 생성합니다.
		mScene = physics->createScene(sceneDesc);

		// PhysX Physics에서 GPU로 작동하는 Scene을 생성합니다 ( Cloth 입자를 위한 )
		sceneDesc.filterShader = CustomGpuSimulationFilterShader;
		sceneDesc.cudaContextManager = mCudaContextManager;
		sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
		mGpuScene = physics->createScene(sceneDesc);
		assert(mScene);
		assert(mGpuScene);

		// PVD 클라이언트에 PhysX Scene 연결 ( Debug )
#ifdef _DEBUG
		mPhysics->SettingPVDClient(mScene);
#endif

		// 매니저 초기화
		if (!mResourceManager->Initialize(mPhysics->GetPhysics()))
		{
			spdlog::warn("[Physics Warrning ({})] ResourceManager Failed Init", __LINE__);
			return false;
		}
		if (!mRigidBodyManager->Initialize(mPhysics->GetPhysics(), mResourceManager, mCollisionDataManager))
		{
			spdlog::warn("[Physics Warrning ({})] RigidBodyManager Failed Init", __LINE__);
			return false;
		}
		if (!mCCTManager->initialize(mScene, mPhysics->GetPhysics(), mCollisionDataManager, mCollisionMatrix))
		{
			spdlog::warn("[Physics Warrning ({})] CCTManager Failed Init", __LINE__);
			return false;
		}
		if (!mCharacterPhysicsManager->initialize(mPhysics->GetPhysics(), mScene, mCollisionDataManager))
		{
			spdlog::warn("[Physics Warrning ({})] CharacterPhysics Failed Init", __LINE__);
			return false;
		}
		if (!mClothManager->Initialize(mPhysics->GetPhysics(), mGpuScene, mCudaContextManager))
		{
			spdlog::warn("[Physics Warrning ({})] ClothManager Failed Init", __LINE__);
			return false;
		}

		// 콜백 함수 등록
		mMyEventCallback->Initialize(mCollisionDataManager);

		return true;
	}

	bool FQPhysics::Update(float deltaTime)
	{
		RemoveActors();

		if (!mRigidBodyManager->Update(mScene, mGpuScene))
			return false;
		if (!mCCTManager->Update(deltaTime))
			return false;
		if (!mCollisionDataManager->Update())
			return false;
		if (!mCharacterPhysicsManager->Update())
			return false;
		if (!mScene->simulate(deltaTime))
			return false;
		if (!mGpuScene->simulate(deltaTime))
			return false;
		if (!mScene->fetchResults(true))
			return false;
		if (!mGpuScene->fetchResults(true))
			return false;
		mMyEventCallback->OnTrigger();

		return true;
	}

	bool FQPhysics::FinalUpdate()
	{
		if (!mRigidBodyManager->FinalUpdate())
			return false;
		if (!mCCTManager->FinalUpdate())
			return false;
		if (!mClothManager->Update())
			return false;

		return true;
	}

	void FQPhysics::SetCallBackFunction(std::function<void(fq::physics::CollisionData, ECollisionEventType)> func)
	{
		// 충돌 콜백 등록
		mMyEventCallback->SetCallbackFunction(func);
	}

	void FQPhysics::SetPhysicsInfo(PhysicsEngineInfo& info)
	{
		// 물리 엔진 세팅 ( 중력, 충돌 매트릭스 )
		mScene->setGravity(physx::PxVec3(info.gravity.x, info.gravity.y, -info.gravity.z));
		memcpy(mCollisionMatrix, info.collisionMatrix, sizeof(int) * COLLISION_MATRIX_SIZE);
		mRigidBodyManager->UpdateCollisionMatrix(mCollisionMatrix);
		mCCTManager->UpdateCollisionMatrix(mCollisionMatrix);
	}

	RayCastOutput FQPhysics::RayCast(const RayCastInput& info, bool isStatic, bool isGPUScene)
	{
		// 기본 설정
		physx::PxVec3 pxOrigin;
		physx::PxVec3 pxDirection;
		CopyDxVec3ToPxVec3(info.origin, pxOrigin);
		CopyDxVec3ToPxVec3(info.direction, pxDirection);

		// 결과 저장 버퍼
		const physx::PxU32 maxHits = 20;
		physx::PxRaycastHit hitBuffer[maxHits];
		physx::PxRaycastBuffer hitBufferStruct(hitBuffer, maxHits);

		// 쿼리 정보 설정
		physx::PxQueryFilterData qfd;
		qfd.data.word0 = info.layerNumber;
		qfd.data.word1 = mCollisionMatrix[info.layerNumber];

		if (isStatic)
		{
			qfd.flags = physx::PxQueryFlag::eSTATIC
				| physx::PxQueryFlag::ePREFILTER
				| physx::PxQueryFlag::eNO_BLOCK
				| physx::PxQueryFlag::eDISABLE_HARDCODED_FILTER; // Physx 핕터형식 적용 X
		}
		else
		{
			qfd.flags = physx::PxQueryFlag::eDYNAMIC  
				| physx::PxQueryFlag::ePREFILTER
				| physx::PxQueryFlag::eNO_BLOCK
				| physx::PxQueryFlag::eDISABLE_HARDCODED_FILTER; // Physx 핕터형식 적용 X
		}

		RaycastQueryFileter queryfilter;
		bool isAnyHit;

		if (isGPUScene)
		{
			isAnyHit = mGpuScene->raycast(pxOrigin
				, pxDirection
				, info.distance
				, hitBufferStruct
				, physx::PxHitFlag::eDEFAULT
				, qfd
				, &queryfilter);
		}
		else
		{
			isAnyHit = mScene->raycast(pxOrigin
				, pxDirection
				, info.distance
				, hitBufferStruct
				, physx::PxHitFlag::eDEFAULT
				, qfd
				, &queryfilter);
		}

		RayCastOutput output;

		if (isAnyHit)
		{
			// Block 정보 저장 
			output.hasBlock = hitBufferStruct.hasBlock;
			if (output.hasBlock)
			{
				const physx::PxRaycastHit& blockHit = hitBufferStruct.block;
				output.blockID = static_cast<CollisionData*>(hitBufferStruct.block.shape->userData)->myId;
				hitBufferStruct.block.position;
				CopyPxVec3ToDxVec3(hitBufferStruct.block.position, output.blockPosition);
			}

			// Hit정보 저장
			unsigned int hitSize = hitBufferStruct.nbTouches;
			hitBufferStruct.block;
			output.hitSize = hitSize;

			for (unsigned int hitNumber = 0; hitNumber < hitSize; hitNumber++)
			{
				const physx::PxRaycastHit& hit = hitBufferStruct.touches[hitNumber];
				physx::PxShape* shape = hit.shape;

				DirectX::SimpleMath::Vector3 position;
				CopyPxVec3ToDxVec3(hit.position, position);
				unsigned int id = static_cast<CollisionData*>(shape->userData)->myId;
				unsigned int layerNumber = static_cast<CollisionData*>(shape->userData)->myLayerNumber;

				output.contectPoints.push_back(position);
				output.id.push_back(id);
			}
		}

		return output;
	}

#pragma region RigidBodyManager
	bool FQPhysics::CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType)
	{
		if (mRigidBodyManager->CreateStaticBody(info, colliderType, mCollisionMatrix))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create StaticBody(Box) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType)
	{
		if (mRigidBodyManager->CreateStaticBody(info, colliderType, mCollisionMatrix))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create StaticBody(Sphere) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType)
	{
		if (mRigidBodyManager->CreateStaticBody(info, colliderType, mCollisionMatrix))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create StaticBody(Capsule) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType)
	{
		if (mRigidBodyManager->CreateStaticBody(info, colliderType, mCollisionMatrix))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create StaticBody(ConvexMesh) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateStaticBody(const TriangleMeshColliderInfo& info, const EColliderType& colliderType)
	{
		if (mRigidBodyManager->CreateStaticBody(info, colliderType, mCollisionMatrix))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create StaticBody(TriangleMesh) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateStaticBody(const HeightFieldColliderInfo& info, const EColliderType& colliderType)
	{
		if (mRigidBodyManager->CreateStaticBody(info, colliderType, mCollisionMatrix))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create StaticBody(HeightField) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType, bool isKinematic)
	{
		if (mRigidBodyManager->CreateDynamicBody(info, colliderType, mCollisionMatrix, isKinematic))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create DynamicBody(Box) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType, bool isKinematic)
	{
		if (mRigidBodyManager->CreateDynamicBody(info, colliderType, mCollisionMatrix, isKinematic))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create DynamicBody(Sphere) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType, bool isKinematic)
	{
		if (mRigidBodyManager->CreateDynamicBody(info, colliderType, mCollisionMatrix, isKinematic))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create DynamicBody(Capsule) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType, bool isKinematic)
	{
		if (mRigidBodyManager->CreateDynamicBody(info, colliderType, mCollisionMatrix, isKinematic))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create DynamicBody(ConvexMesh) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateDynamicBody(const TriangleMeshColliderInfo& info, const EColliderType& colliderType, bool isKinematic)
	{
		if (mRigidBodyManager->CreateDynamicBody(info, colliderType, mCollisionMatrix, isKinematic))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create DynamicBody(TriangleMesh) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::CreateDynamicBody(const HeightFieldColliderInfo& info, const EColliderType& colliderType, bool isKinematic)
	{
		if (mRigidBodyManager->CreateDynamicBody(info, colliderType, mCollisionMatrix, isKinematic))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Create DynamicBody(HeightField) ID : {}", __LINE__, info.colliderInfo.id);
			return false;
		}
	}
	bool FQPhysics::RemoveRigidBody(const unsigned int& id)
	{
		if (mRigidBodyManager->RemoveRigidBody(id, mScene, mActorsToRemove))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Remove RigidBody ID : {}", __LINE__, id);
			return false;
		}
	}
	bool FQPhysics::RemoveAllRigidBody()
	{
		if (mRigidBodyManager->RemoveAllRigidBody(mScene, mActorsToRemove))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] : Failed Remove All RigidBody", __LINE__);
			return false;
		}
	}
	RigidBodyGetSetData FQPhysics::GetRigidBodyData(const unsigned int& id)
	{
		RigidBodyGetSetData rigidBodyData;
		mRigidBodyManager->GetRigidBodyData(id, rigidBodyData);

		return rigidBodyData;
	}
	bool FQPhysics::SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData)
	{
		if (mRigidBodyManager->SetRigidBodyData(id, rigidBodyData, mCollisionMatrix))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] Failed SetRigidBodyData ID : {}", __LINE__, id);
			return false;
		}
	}
	bool FQPhysics::ChangeScene()
	{
		RemoveAllController();
		return false;
	}
	const std::unordered_map<unsigned int, PolygonMesh>& FQPhysics::GetDebugPolygon()
	{
		return mRigidBodyManager->GetDebugPolygon();
	}
	const std::unordered_map<unsigned int, std::vector<unsigned int>>& FQPhysics::GetDebugTriangleIndiecs()
	{
		return mRigidBodyManager->GetDebugTriangleIndiecs();
	}
	const std::unordered_map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>>& FQPhysics::GetDebugTriangleVertices()
	{
		return mRigidBodyManager->GetDebugTriangleVertices();
	}
	const std::unordered_map<unsigned int, std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>>& FQPhysics::GetDebugHeightField()
	{
		return mRigidBodyManager->GetDebugHeightField();
	}
#pragma endregion

#pragma region CCTManager
	bool FQPhysics::CreateCCT(const CharacterControllerInfo& controllerInfo, const CharacterMovementInfo& movementInfo)
	{
		if (mCCTManager->CreateCCT(controllerInfo, movementInfo))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Phyiscs Warrning ({})] Failed Create CCT(Character Controller) ID : {}", __LINE__, controllerInfo.id);
			return false;
		}
	}
	bool FQPhysics::RemoveController(const unsigned int& id)
	{
		if (mCCTManager->RemoveController(id))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] Failed Remove CCT(Character Controller) ID : {}", __LINE__, id);
			return false;
		}
	}
	bool FQPhysics::RemoveAllController()
	{
		if (mCCTManager->RemoveAllController())
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] Failed Remove All CCT(Character Controller)", __LINE__);
			return false;
		}
	}
	bool FQPhysics::AddInputMove(const CharacterControllerInputInfo& info)
	{
		if (mCCTManager->AddInputMove(info))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] Failed Input. Input ID : {}", __LINE__, info.id);
			return false;
		}
	}

	CharacterControllerGetSetData FQPhysics::GetCharacterControllerData(const unsigned int& id)
	{
		CharacterControllerGetSetData data;
		mCCTManager->GetCharacterControllerData(id, data);
		return data;
	}
	CharacterMovementGetSetData FQPhysics::GetCharacterMovementData(const unsigned int& id)
	{
		CharacterMovementGetSetData data;
		mCCTManager->GetCharacterMovementData(id, data);
		return data;
	}
	void FQPhysics::SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData)
	{
		mCCTManager->SetCharacterControllerData(id, controllerData);
	}
	void FQPhysics::SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData)
	{
		mCCTManager->SetCharacterMovementData(id, movementData);
	}
#pragma endregion

#pragma region CharacterPhysicsManager
	bool FQPhysics::CreateCharacterphysics(const ArticulationInfo& info)
	{
		return mCharacterPhysicsManager->CreateCharacterphysics(info);
	}
	bool FQPhysics::RemoveArticulation(const unsigned int& id)
	{
		return mCharacterPhysicsManager->RemoveArticulation(id);
	}
	bool FQPhysics::AddArticulationLink(unsigned int id, LinkInfo& info, const DirectX::SimpleMath::Vector3& extent)
	{
		return mCharacterPhysicsManager->AddArticulationLink(id, info, mCollisionMatrix, extent);
	}
	bool FQPhysics::AddArticulationLink(unsigned int id, LinkInfo& info, const float& radius)
	{
		return mCharacterPhysicsManager->AddArticulationLink(id, info, mCollisionMatrix, radius);
	}
	bool FQPhysics::AddArticulationLink(unsigned int id, LinkInfo& info, const float& halfHeight, const float& radius)
	{
		return mCharacterPhysicsManager->AddArticulationLink(id, info, mCollisionMatrix, halfHeight, radius);
	}
	bool FQPhysics::AddArticulationLink(unsigned int id, LinkInfo& info)
	{
		return mCharacterPhysicsManager->AddArticulationLink(id, info, mCollisionMatrix);
	}

	ArticulationGetData FQPhysics::GetArticulationData(const unsigned int& id)
	{
		ArticulationGetData data;
		mCharacterPhysicsManager->GetArticulationData(id, data);
		return data;
	}

	void FQPhysics::SetArticulationData(const unsigned int& id, const ArticulationSetData& articulationData)
	{
		mCharacterPhysicsManager->SetArticulationData(id, articulationData, mCollisionMatrix);
	}
	unsigned int FQPhysics::GetArticulationCount()
	{
		return mCharacterPhysicsManager->GetCharacterPhysicsCount();
	}
#pragma endregion

#pragma region PhysicsClothManager
	bool FQPhysics::CreateCloth(const Cloth::CreateClothData& info)
	{
		if (mClothManager->CreateCloth(info, mCollisionMatrix))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] Failed Create Cloth ID : {}", __LINE__, info.id);
			return false;
		}
	}
	bool FQPhysics::GetClothData(unsigned int id, Cloth::GetSetClothData& data)
	{
		if (mClothManager->GetClothData(id, data))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] Failed Get Cloth Data! ID : {}", __LINE__, id);
			return false;
		}
	}
	bool FQPhysics::SetClothData(unsigned int id, const Cloth::GetSetClothData& data)
	{
		if (mClothManager->SetClothData(id, data))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] Failed Set Cloth Data! ID : {}", __LINE__, id);
			return false;
		}
	}
	bool FQPhysics::RemoveCloth(unsigned int id)
	{
		if (mClothManager->RemoveCloth(id, mActorsToRemove))
		{
			return true;
		}
		else
		{
			spdlog::warn("[Physics Warrning ({})] Failed Remove Cloth ID : {}", __LINE__, id);
			return false;
		}
	}
#pragma endregion

	bool FQPhysics::HasConvexMeshResource(const unsigned int& hash)
	{
		auto resource = mResourceManager->Find<ConvexMeshResource>(hash);

		if (resource)
			return true;
		else
			return false;
	}

#pragma  region spdlog
	std::shared_ptr<spdlog::logger> FQPhysics::SetUpLogger(std::vector<spdlog::sink_ptr> sinks)
	{
		auto logger = std::make_shared<spdlog::logger>("Physics",
			std::begin(sinks),
			std::end(sinks));

		spdlog::set_default_logger(logger);
		return logger;
	}

	void FQPhysics::RemoveActors()
	{
		for (auto removeActor : mActorsToRemove)
		{
			mScene->removeActor(*removeActor);
		}
		mActorsToRemove.clear();
	}
#pragma endregion
}