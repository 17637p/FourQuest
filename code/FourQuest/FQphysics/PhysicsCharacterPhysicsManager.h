#pragma once

#include <map>
#include <memory>
#include <unordered_map>

namespace fq::physics
{
	class CharacterPhysics;

	class PhysicsCharacterPhysicsManager
	{
	public:
		PhysicsCharacterPhysicsManager();
		~PhysicsCharacterPhysicsManager();
		

	private:
		std::unordered_map<unsigned int, std::shared_ptr<CharacterPhysics>> mCharacterPhysicsContainer;

	};
}