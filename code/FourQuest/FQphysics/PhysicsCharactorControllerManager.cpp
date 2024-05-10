#include "PhysicsCharactorControllerManager.h"

#include <physx\PxPhysicsApi.h>

#include "CharacterController.h"
#include "PhysicsCCTHitCallback.h"
#include "PlayerCharacterController.h"
#include "CharacterMovement.h"

namespace fq::physics
{
	PhysicsCharactorControllerManager::PhysicsCharactorControllerManager()
		: mPhysics(nullptr)
		, mMaterial(nullptr)
		, mCCTManager(nullptr)
		, mCCTmap()
		, mCollisionDataContainer()
	{
	}

	PhysicsCharactorControllerManager::~PhysicsCharactorControllerManager()
	{
		mCCTmap.clear();
		PX_RELEASE(mMaterial);
		PX_RELEASE(mCCTManager);
		mCollisionDataContainer.clear();
	}

	bool PhysicsCharactorControllerManager::initialize(physx::PxScene* scene, physx::PxPhysics* physics)
	{
		mCCTManager = PxCreateControllerManager(*scene);
		assert(mCCTManager);

		mPhysics = physics;
		mMaterial = mPhysics->createMaterial(1.f, 1.f, 1.f);

		return true;
	}

	bool PhysicsCharactorControllerManager::Update(float deltaTime)
	{
		for (const auto& controller : mCCTmap)
		{
			if (!controller.second->Update(deltaTime))
				return false;
		}

		return true;
	}

	bool PhysicsCharactorControllerManager::CreateCCT(const CharacterControllerInfo& controllerInfo, const CharacterMovementInfo& movementInfo, int* collisionMatrix)
	{
		std::shared_ptr<PlayerCharacterController> controller = std::make_shared<PlayerCharacterController>();
		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();
		if (!controller->Initialize(controllerInfo, movementInfo, mCCTManager, mMaterial, collisionData, collisionMatrix)) return false;

		mCollisionDataContainer.insert(std::make_pair(collisionData->myId, collisionData));
		mCCTmap.insert(std::make_pair(controller->GetID(), controller));
		return true;
	}

	void PhysicsCharactorControllerManager::AddInputMove(const unsigned int& id, const DirectX::SimpleMath::Vector3& input)
	{
		std::shared_ptr<CharacterController>& characterController = mCCTmap.find(id)->second;
		characterController->AddMovementInput(input);
	}

	bool PhysicsCharactorControllerManager::RemoveController(const unsigned int& id)
	{
		auto controller = mCCTmap.find(id);
		if (controller != mCCTmap.end())
		{
			mCCTmap.erase(controller);
			return true;
		}

		return false;
	}

	void PhysicsCharactorControllerManager::UpdateCollisionMatrix(int* collisionMatrix)
	{
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

	void PhysicsCharactorControllerManager::GetCharacterControllerData(const unsigned int& id, CharacterControllerGetSetData& data)
	{
		auto& controller = mCCTmap.find(id)->second;
		physx::PxController* pxController = controller->GetPxController();

		controller->GetPosition(data.position);
	}
	void PhysicsCharactorControllerManager::GetCharacterMovementData(const unsigned int& id, CharacterMovementGetSetData& data)
	{
		auto& controller = mCCTmap.find(id)->second;
		std::shared_ptr<CharacterMovement> movement = controller->GetCharacterMovement();

		data.velocity = movement->GetDisplacementVector();
		data.isFall = movement->GetIsFall();
	}
	void PhysicsCharactorControllerManager::SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData)
	{
		auto& controller = mCCTmap.find(id)->second;
		physx::PxController* pxController = controller->GetPxController();

		controller->SetPosition(controllerData.position);
	}
	void PhysicsCharactorControllerManager::SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData)
	{
		auto& controller = mCCTmap.find(id)->second;
		std::shared_ptr<CharacterMovement> movement = controller->GetCharacterMovement();

		movement->SetIsFall(movementData.isFall);
		movement->SetVelocity(movementData.velocity);
	}
}
