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

		bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const DirectX::SimpleMath::Vector3& extent);
		bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const float& radius);
		bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const float& halfHeight, const float& radius);

		inline std::shared_ptr<CharacterPhysics> GetCharacterPhysics(unsigned int id);

	private:
		physx::PxPhysics* mPhysics;
		physx::PxScene* mScene;

		std::unordered_map<unsigned int, std::shared_ptr<CharacterPhysics>> mCharacterPhysicsContainer;
		std::unordered_map<unsigned int, std::shared_ptr<CollisionData>>	mCollisionDataConttainer;
	};

	std::shared_ptr<CharacterPhysics> PhysicsCharacterPhysicsManager::GetCharacterPhysics(unsigned int id)
	{
		return mCharacterPhysicsContainer.find(id)->second;
	}
}