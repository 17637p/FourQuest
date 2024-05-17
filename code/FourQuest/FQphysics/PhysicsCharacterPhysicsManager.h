#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <physx\PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterPhysics;

	class PhysicsCharacterPhysicsManager
	{
	public:
		PhysicsCharacterPhysicsManager();
		~PhysicsCharacterPhysicsManager();
		
		bool initialize(physx::PxPhysics* physics, physx::PxScene* scene);

		bool CreateCharacterphysics(const CharacterPhysicsInfo& info);

		std::shared_ptr<CharacterPhysics> GetCharacterPhysics();

	private:
		physx::PxPhysics* mPhysics;
		physx::PxScene* mScene;

		std::unordered_map<unsigned int, std::shared_ptr<CharacterPhysics>> mCharacterPhysicsContainer;
		std::unordered_map<unsigned int, std::shared_ptr<CollisionData>>	mCollisionDataConttainer;
	};
}