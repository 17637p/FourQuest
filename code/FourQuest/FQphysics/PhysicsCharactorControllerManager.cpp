#include "PhysicsCharactorControllerManager.h"

#include <physx\PxPhysicsApi.h>

#include "CharacterController.h"

namespace fq::physics
{
	PhysicsCharactorControllerManager::PhysicsCharactorControllerManager()
	{
	}

	PhysicsCharactorControllerManager::~PhysicsCharactorControllerManager()
	{
	}

	bool PhysicsCharactorControllerManager::initialize(physx::PxScene* scene)
	{
		mCCTManager = PxCreateControllerManager(*scene);
		assert(mCCTManager);

		return true;
	}

	bool PhysicsCharactorControllerManager::CreateCCT(CharacterControllerInfo controllerInfo, CharacterMovementInfo movementInfo)
	{
		//std::shared_ptr<CharacterController> controller = std::make_shared<

		return true;
	}
}
