#include "PhysicsCharacterPhysicsManager.h"

#include "CharacterPhysics.h"

namespace fq::physics
{
	PhysicsCharacterPhysicsManager::PhysicsCharacterPhysicsManager()
		: mPhysics(nullptr)
		, mScene(nullptr)
		, mCharacterPhysicsContainer()
		, mCollisionDataManager()
	{
	}

	PhysicsCharacterPhysicsManager::~PhysicsCharacterPhysicsManager()
	{
	}

	bool PhysicsCharacterPhysicsManager::initialize(physx::PxPhysics* physics, physx::PxScene* scene, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager)
	{
		mCollisionDataManager = collisionDataManager;
		mPhysics = physics;
		mScene = scene;

		return true;
	}

	bool PhysicsCharacterPhysicsManager::CreateCharacterphysics(const ArticulationInfo& info)
	{
		if (mCharacterPhysicsContainer.find(info.id) != mCharacterPhysicsContainer.end())
			return false;

		std::shared_ptr<CharacterPhysics> characterPhysics = std::make_shared<CharacterPhysics>();

		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();
		collisionData->myId = info.id;
		collisionData->myLayerNumber = info.layerNumber;
		mCollisionDataManager.lock()->Create(info.id, collisionData);

		characterPhysics->Initialize(info, mPhysics, collisionData);
		mCharacterPhysicsContainer.insert(std::make_pair(info.id, characterPhysics));

		return true;
	}

	bool PhysicsCharacterPhysicsManager::SimulationCharacter(unsigned int id)
	{
		assert(mCharacterPhysicsContainer.find(id) != mCharacterPhysicsContainer.end());

		std::shared_ptr<CharacterPhysics> characterPhysics = mCharacterPhysicsContainer.find(id)->second;
		return mScene->addArticulation(*characterPhysics->GetPxArticulation());
	}

	bool PhysicsCharacterPhysicsManager::ChangeScene()
	{


		return true;
	}
}
