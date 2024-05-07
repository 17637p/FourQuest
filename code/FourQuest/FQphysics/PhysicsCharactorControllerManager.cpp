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


		return true;
	}

	bool PhysicsCharactorControllerManager::CreateCCT(CharacterControllerInfo controllerInfo, CharacterMovementInfo movementInfo)
	{


		return true;
	}
}
