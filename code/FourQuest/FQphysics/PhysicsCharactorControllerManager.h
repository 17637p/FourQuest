#pragma once

#include <memory>
#include <map>
#include <physx\PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterController;

	class PhysicsCharactorControllerManager
	{
	public:
		PhysicsCharactorControllerManager();
		~PhysicsCharactorControllerManager();

		bool initialize(physx::PxScene* scene);

		bool CreateCCT(CharacterControllerInfo controllerInfo, CharacterMovementInfo movementInfo);

	private:
		physx::PxControllerManager* mCCTManager;

		std::unordered_map<int, std::shared_ptr<CharacterController>> mCCTmap;
	};
}