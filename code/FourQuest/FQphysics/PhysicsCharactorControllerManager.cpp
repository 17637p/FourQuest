#include "PhysicsCharactorControllerManager.h"

#include <PxPhysicsApi.h>

#include "CharacterController.h"
#include "PhysicsCCTHitCallback.h"
#include "PlayerCharacterController.h"
#include "CharacterMovement.h"
#include "PhysicsCollisionDataManager.h"

namespace fq::physics
{
	PhysicsCharactorControllerManager::PhysicsCharactorControllerManager()
		: mPhysics(nullptr)
		, mMaterial(nullptr)
		, mCCTManager(nullptr)
		, mCollisionMatrix(nullptr)
		, mCCTmap{}
		, mWaitCharacterControllerContainer{}
		, mUpComingCharacterControllerContainer{}
	{
	}

	PhysicsCharactorControllerManager::~PhysicsCharactorControllerManager()
	{
		mCCTmap.clear();
		mWaitCharacterControllerContainer.clear();
		mUpComingCharacterControllerContainer.clear();
		PX_RELEASE(mMaterial);
		PX_RELEASE(mCCTManager);
	}

	bool PhysicsCharactorControllerManager::initialize(
		physx::PxScene* scene, 
		physx::PxPhysics* physics, 
		std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager,
		int* collisionMatrix
	)
	{
		mPhysics = physics;
		mCollisionDataManager = collisionDataManager;
		mCollisionMatrix = collisionMatrix;
		mCCTManager = PxCreateControllerManager(*scene);
		assert(mCCTManager);

		mMaterial = mPhysics->createMaterial(1.f, 1.f, 1.f);
		return true;
	}

	bool PhysicsCharactorControllerManager::Update(float deltaTime)
	{
		if (!updateCCT(deltaTime)) 
			return false;
		if (!CreateCCT())
			return false;

		return true;
	}

	bool PhysicsCharactorControllerManager::updateCCT(float deltaTime)
	{
		for (const auto& controller : mCCTmap)
		{
			if (!controller.second->Update(deltaTime))
				return false;
		}
		return true;
	}

	bool PhysicsCharactorControllerManager::FinalUpdate()
	{
		return true;
	}

	bool PhysicsCharactorControllerManager::AddInputMove(const CharacterControllerInputInfo& info)
	{
		if (mCCTmap.find(info.id) == mCCTmap.end())
			return false;

		std::shared_ptr<CharacterController>& characterController = mCCTmap.find(info.id)->second;
		characterController->AddMovementInput(info.input, info.isDynamic);
		return true;
	}

	void PhysicsCharactorControllerManager::UpdateCollisionMatrix(int* collisionMatrix)
	{
		mCollisionMatrix = collisionMatrix;

		// 수정된 충돌 매트릭스 세팅
		for (auto& controller : mCCTmap)
		{
			physx::PxFilterData filterData;
			filterData.word0 = controller.first;
			filterData.word1 = collisionMatrix[controller.first];

			physx::PxShape* shape;
			controller.second->GetPxController()->getActor()->getShapes(&shape, 1);
			shape->setSimulationFilterData(filterData);
		}
	}

#pragma region CreateAndRemoveCCT
	bool PhysicsCharactorControllerManager::CreateCCT(CharacterControllerInfo controllerInfo, CharacterMovementInfo movementInfo)
	{
		mWaitCharacterControllerContainer.push_back(std::make_pair(controllerInfo, movementInfo));
		return true;
	}

	bool PhysicsCharactorControllerManager::CreateCCT()
	{
		// 캐릭터 컨트롤러 생성
		for (auto& [controllerInfo, movementInfo] : mUpComingCharacterControllerContainer)
		{
			std::shared_ptr<PlayerCharacterController> controller = std::make_shared<PlayerCharacterController>();
			std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();
			if (!controller->Initialize(controllerInfo, movementInfo, mCCTManager, mMaterial, collisionData, mCollisionMatrix)) return false;

			mCollisionDataManager.lock()->Create(controllerInfo.id, collisionData);
			mCCTmap.insert(std::make_pair(controller->GetID(), controller));
		}
		mUpComingCharacterControllerContainer.clear();

		// 한턴 뒤 생성할 캐릭터 컨트롤러 보관
		for (auto& characterControllerInfo : mWaitCharacterControllerContainer)
		{
			mUpComingCharacterControllerContainer.push_back(characterControllerInfo);
		}
		mWaitCharacterControllerContainer.clear();

		return true;
	}

	bool PhysicsCharactorControllerManager::RemoveController(const unsigned int& id)
	{
		// 캐릭터 컨트롤러 삭제
		auto controllerIter = mCCTmap.find(id);

		if (controllerIter != mCCTmap.end())
		{
			mCCTmap.erase(controllerIter);
			return true;
		}
 
		return false;
	}
	bool PhysicsCharactorControllerManager::RemoveAllController()
	{
		mCCTmap.clear();
		mUpComingCharacterControllerContainer.clear();
		mWaitCharacterControllerContainer.clear();
		return true;
	}
#pragma endregion

#pragma region GetSetFunction
	void PhysicsCharactorControllerManager::GetCharacterControllerData(const unsigned int& id, CharacterControllerGetSetData& data)
	{
		// 생성했거나 생성할 캐릭터 컨트롤러 데이터 반환
		if (mCCTmap.find(id) != mCCTmap.end())
		{
			auto& controller = mCCTmap.find(id)->second;
			physx::PxController* pxController = controller->GetPxController();

			controller->GetPosition(data.position);
		}
		else
		{
			for (auto& [controllor, movement] : mUpComingCharacterControllerContainer)
			{
				if (controllor.id == id)
				{
					data.position = controllor.position;
					return;
				}
			}

			for (auto& [controller, movement] : mWaitCharacterControllerContainer)
			{
				if (controller.id == id)
				{
					data.position = controller.position;
					return;
				}
			}
		}
		
	}
	void PhysicsCharactorControllerManager::GetCharacterMovementData(const unsigned int& id, CharacterMovementGetSetData& data)
	{
		if (mCCTmap.find(id) != mCCTmap.end())
		{
			auto& controller = mCCTmap.find(id)->second;
			std::shared_ptr<CharacterMovement> movement = controller->GetCharacterMovement();

			data.velocity = movement->GetDisplacementVector();
			data.isFall = movement->GetIsFall();
			data.maxSpeed = movement->GetMaxSpeed();
		}

	}
	void PhysicsCharactorControllerManager::SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData)
	{
		// 생성했거나 생성할 캐릭터 컨트롤러 데이터 세팅
		if (mCCTmap.find(id) != mCCTmap.end())
		{
			auto& controller = mCCTmap.find(id)->second;
			physx::PxController* pxController = controller->GetPxController();

			mCCTmap.find(id)->second->ChangeLayerNumber(controllerData.myLayerNumber, mCollisionMatrix, mCollisionDataManager);
			controller->SetPosition(controllerData.position);
		}
		else
		{
			for (auto& [controllor, movement] : mUpComingCharacterControllerContainer)
			{
				if (controllor.id == id)
				{
					controllor.position = controllerData.position;
				}
			}

			for (auto& [controller, movement] : mWaitCharacterControllerContainer)
			{
				if (controller.id == id)
				{
					controller.position = controllerData.position;
					return;
				}
			}
		}
	}
	void PhysicsCharactorControllerManager::SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData)
	{
		if (mCCTmap.find(id) != mCCTmap.end())
		{
			auto& controller = mCCTmap.find(id)->second;
			std::shared_ptr<CharacterMovement> movement = controller->GetCharacterMovement();

			controller->SetMoveRestriction(movementData.restriction);
			movement->SetIsFall(movementData.isFall);
			movement->SetVelocity(movementData.velocity);
			movement->SetMaxSpeed(movementData.maxSpeed);
			movement->SetAcceleration(movementData.acceleration);
		}
	}
#pragma endregion
}
