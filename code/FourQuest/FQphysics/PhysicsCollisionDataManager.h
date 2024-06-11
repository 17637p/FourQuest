#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class PhysicsCollisionDataManager
	{
	public:
		PhysicsCollisionDataManager();
		~PhysicsCollisionDataManager();

		bool Update();
		void Create(unsigned int id, std::shared_ptr<CollisionData> collisionData);
		void Remove(unsigned int id);

		std::shared_ptr<CollisionData> FindCollisionData(unsigned int id);

	public:
		std::unordered_map<unsigned int, std::shared_ptr<CollisionData>> mCollisionDataContainer;
		std::vector<unsigned int> mRemoveID;
	};
}