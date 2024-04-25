#include "FQPhysics.h"

#include "Physics.h"
#include "PhysicsRigidBodyManager.h"
#include "PhysicsSimulationEventCallback.h"

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
				pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
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
				| physx::PxPairFlag::eCONTACT_EVENT_POSE;
			return physx::PxFilterFlag::eDEFAULT;
		}
		else
		{
			pairFlags &= ~physx::PxPairFlag::eCONTACT_DEFAULT; // 충돌 행동 비허용
			return physx::PxFilterFlag::eSUPPRESS;
		}
	}

	FQPhysics::FQPhysics()
		: mPhysics(std::make_shared<Physics>())
		, mRigidBodyManager(std::make_shared<PhysicsRigidBodyManager>())
		, mMyEventCallback(std::make_shared<PhysicsSimulationEventCallback>())
		, mScene(nullptr)
		, mCollisionMatrix{}
	{
	}

	FQPhysics::~FQPhysics()
	{
		PX_RELEASE(mScene);
	}

	bool FQPhysics::Initialize(PhysicsEngineInfo& info)
	{
		mPhysics->Initialize();
		if (!mRigidBodyManager->Initialize(mPhysics->GetPhysics()))
			return false;

		physx::PxPhysics* physics = mPhysics->GetPhysics();

		// PhysX 시뮬레이션을 위한 Scene을 설정합니다.
		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale()); // Scene을 생성할 때 물리적인 허용 오차 스케일을 설정합니다.

		// 중력을 설정합니다.
		sceneDesc.gravity.x = info.gravity.x;
		sceneDesc.gravity.y = info.gravity.y;
		sceneDesc.gravity.z = -info.gravity.z;
		mCollisionMatrix = info.collisionMatrix;

		// 충돌 쌍 플래그입니다.
		physx::PxPairFlags pairFlags = physx::PxPairFlags();
		
		// Scene 설명자에 CPU 디스패처와 필터 셰이더를 설정합니다.
		sceneDesc.cpuDispatcher = mPhysics->GetDispatcher();
		sceneDesc.filterShader = CustomSimulationFilterShader;
		sceneDesc.simulationEventCallback = mMyEventCallback.get();

		// PhysX Phsics에서 PhysX의 Scene을 생성합니다.
		mScene = physics->createScene(sceneDesc);
		assert(mScene);

		// PVD 클라이언트에 PhysX Scene 연결
#ifdef _DEBUG
		mPhysics->SettingPVDClient(mScene);
#endif

		return true;
	}

	bool FQPhysics::Update(float deltaTime)
	{
		if (!mRigidBodyManager->Update(mScene, mCollisionMatrix))
			return false;

		if (!mScene->simulate(deltaTime))
			return false;
		if (!mScene->fetchResults(true))
			return false;

		return true;
	}

#pragma region CreateStaticAndDynamicBody

	bool FQPhysics::CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType)
	{
		if (!mRigidBodyManager->CreateStaticBody(info, colliderType))
			return false;

		return true;
	}
	bool FQPhysics::CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType)
	{
		if (!mRigidBodyManager->CreateStaticBody(info, colliderType))
			return false;

		return true;
	}
	bool FQPhysics::CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType)
	{
		if (!mRigidBodyManager->CreateStaticBody(info, colliderType))
			return false;

		return true;
	}
	bool FQPhysics::CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType)
	{
		if (!mRigidBodyManager->CreateStaticBody(info, colliderType))
			return false;

		return true;
	}
	bool FQPhysics::CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType)
	{
		if (!mRigidBodyManager->CreateDynamicBody(info, colliderType))
			return false;

		return true;
	}
	bool FQPhysics::CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType)
	{
		if (!mRigidBodyManager->CreateDynamicBody(info, colliderType))
			return false;

		return true;
	}
	bool FQPhysics::CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType)
	{
		if (!mRigidBodyManager->CreateDynamicBody(info, colliderType))
			return false;

		return true;
	}
	bool FQPhysics::CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType)
	{
		if (!mRigidBodyManager->CreateDynamicBody(info, colliderType))
			return false;

		return true;
	}

#pragma endregion

	bool FQPhysics::RemoveRigidBody(unsigned int id)
	{
		return false;
	}

	bool FQPhysics::RemoveAllRigidBody()
	{
		return false;
	}
}