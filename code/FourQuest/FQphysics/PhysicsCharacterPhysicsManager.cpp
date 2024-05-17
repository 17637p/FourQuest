#include "PhysicsCharacterPhysicsManager.h"

namespace fq::physics
{
	PhysicsCharacterPhysicsManager::PhysicsCharacterPhysicsManager()
	{
	}

	PhysicsCharacterPhysicsManager::~PhysicsCharacterPhysicsManager()
	{
	}

	bool PhysicsCharacterPhysicsManager::initialize(physx::PxPhysics* physics, physx::PxScene* scene)
	{
		mPhysics = physics;
		mScene = scene;

		return true;
	}

	bool PhysicsCharacterPhysicsManager::CreateCharacterphysics(const CharacterPhysicsInfo& info)
	{
		return false;
	}


}
