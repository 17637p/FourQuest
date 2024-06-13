#include "PhysicsCollisionDataManager.h"

namespace fq::physics
{
	PhysicsCollisionDataManager::PhysicsCollisionDataManager()
		: mCollisionDataContainer()
		, mRemoveID()
	{
	}

	PhysicsCollisionDataManager::~PhysicsCollisionDataManager()
	{
		mCollisionDataContainer.clear();
		mRemoveID.clear();
	}

	bool PhysicsCollisionDataManager::Update()
	{
		for (unsigned int& removeID : mRemoveID)
		{
			auto iter = mCollisionDataContainer.find(removeID);

			if (iter != mCollisionDataContainer.end())
				mCollisionDataContainer.erase(iter);
		}
		mRemoveID.clear();

		for (auto& iter : mCollisionDataContainer)
		{
			if (iter.second->isDead == true)
			{
				mRemoveID.push_back(iter.first);
			}
		}

		return true;
	}

	void PhysicsCollisionDataManager::Create(const unsigned int& id, std::shared_ptr<CollisionData> collisionData)
	{
		mCollisionDataContainer.insert(std::make_pair(id, collisionData));
	}

	void PhysicsCollisionDataManager::Remove(const unsigned int& id)
	{
		for (unsigned int& removeID : mRemoveID)
		{
			if (removeID == id)
			{
				return;
			}
		}

		mRemoveID.push_back(id);
	}
	std::shared_ptr<CollisionData> PhysicsCollisionDataManager::FindCollisionData(const unsigned int& id)
	{
		if (mCollisionDataContainer.find(id) == mCollisionDataContainer.end())
			return nullptr;

		return mCollisionDataContainer.find(id)->second;
	}
}