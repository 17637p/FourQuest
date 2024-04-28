#include "FQPhysics.h"

#include "Physics.h"
#include "PhysicsRigidBodyManager.h"
#include "PhysicsSimulationEventCallback.h"

namespace fq::physics
{ 
	/// <summary>
	/// �浹 �ݹ� �Լ�
	/// <summary>
	physx::PxFilterFlags CustomSimulationFilterShader(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		/// <summary>
		/// �ֿ� ���� CCD�� Ȱ��ȭ�ϰ� �ʱ� �� CCD ����ó�� ���� ����ó ������ ��û�մϴ�.
		/// ���� ������ ������ �����ϰ� �����ڿ��� ������ �����մϴ�
		/// ������ �� ��� ���մϴ�.
		/// <summary>

		// ���� ���̴� ���� ( Ʈ���� )
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

		// ���� ������ �浹 üũ ( �ùķ��̼� )
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
			pairFlags &= ~physx::PxPairFlag::eCONTACT_DEFAULT; // �浹 �ൿ �����
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

		// PhysX �ùķ��̼��� ���� Scene�� �����մϴ�.
		physx::PxSceneDesc sceneDesc(physics->getTolerancesScale()); // Scene�� ������ �� �������� ��� ���� �������� �����մϴ�.

		// �߷��� �����մϴ�.
		sceneDesc.gravity.x = info.gravity.x;
		sceneDesc.gravity.y = info.gravity.y;
		sceneDesc.gravity.z = -info.gravity.z;
		mCollisionMatrix = info.collisionMatrix;

		// �浹 �� �÷����Դϴ�.
		physx::PxPairFlags pairFlags = physx::PxPairFlags();
		
		// Scene �����ڿ� CPU ����ó�� ���� ���̴��� �����մϴ�.
		sceneDesc.cpuDispatcher = mPhysics->GetDispatcher();
		sceneDesc.filterShader = CustomSimulationFilterShader;
		sceneDesc.simulationEventCallback = mMyEventCallback.get();

		// PhysX Phsics���� PhysX�� Scene�� �����մϴ�.
		mScene = physics->createScene(sceneDesc);
		assert(mScene);

		// PVD Ŭ���̾�Ʈ�� PhysX Scene ���� ( Debug )
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

	bool FQPhysics::FinalUpdate()
	{
		if (!mRigidBodyManager->FinalUpdate())
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

#pragma region RemoveRigidBody

	bool FQPhysics::RemoveRigidBody(unsigned int id)
	{
		if (!mRigidBodyManager->RemoveRigidBody(id, mScene))
			return false;

		return true;
	}

	bool FQPhysics::RemoveAllRigidBody()
	{
		if (!mRigidBodyManager->RemoveAllRigidBody(mScene))
			return false;

		return true;
	}

#pragma endregion

#pragma region SetCallBackFunction

	void FQPhysics::SetCallBackFunction(std::function<void(fq::physics::CollisionData, ECollisionEventType)> func)
	{
		mMyEventCallback->SetCallbackFunction(func);
	}

#pragma endregion

#pragma region SetPhysicsInfo

	void FQPhysics::SetPhysicsInfo(PhysicsEngineInfo info)
	{
		mScene->setGravity(physx::PxVec3(info.gravity.x, info.gravity.y, info.gravity.z));
		mCollisionMatrix = info.collisionMatrix;
		mRigidBodyManager->UpdateCollisionMatrix(mCollisionMatrix);
	}

#pragma endregion

	DirectX::SimpleMath::Matrix FQPhysics::GetRigidBodyMatrix(unsigned int id)
	{
		DirectX::SimpleMath::Matrix dxMatrix;
		mRigidBodyManager->GetRigidBodyMatrix(id, dxMatrix);

		return dxMatrix;
	}

	const std::set<std::shared_ptr<DirectX::BoundingOrientedBox>>& FQPhysics::GetDebugBox()
	{
		return mRigidBodyManager->GetDebugBox();
	}
	const std::set<std::shared_ptr<DirectX::BoundingSphere>>& FQPhysics::GetDebugShere()
	{
		return mRigidBodyManager->GetDebugShere();
	}
	const std::set<std::shared_ptr<std::vector<std::vector<DirectX::SimpleMath::Vector3>>>>& FQPhysics::GetDebugPolygon()
	{
		return mRigidBodyManager->GetDebugPolygon();
	}
}