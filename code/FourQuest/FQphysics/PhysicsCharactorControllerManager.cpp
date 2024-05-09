#include "PhysicsCharactorControllerManager.h"

#include <physx\PxPhysicsApi.h>

#include "CharacterController.h"
#include "PhysicsCCTHitCallback.h"
#include "PlayerCharacterController.h"

namespace fq::physics
{
	PhysicsCharactorControllerManager::PhysicsCharactorControllerManager()
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

		// 콜리전 데이터 없애기
		for (auto& iter : mCollisionDataContainer)
		{
			auto collision = mCCTmap.find(iter.first);
			if (collision != mCCTmap.end())
			{
				mCollisionDataContainer.erase(mCollisionDataContainer.find(iter.first));
			}
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
}
