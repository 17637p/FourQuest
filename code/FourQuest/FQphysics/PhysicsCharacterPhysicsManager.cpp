#include "PhysicsCharacterPhysicsManager.h"

#include "CharacterPhysics.h"

namespace fq::physics
{
	PhysicsCharacterPhysicsManager::PhysicsCharacterPhysicsManager()
		: mPhysics(nullptr)
		, mScene(nullptr)
		, mCharacterPhysicsContainer()
		, mCollisionDataConttainer()
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
		if (mCharacterPhysicsContainer.find(info.id) != mCharacterPhysicsContainer.end())
			return false;

		std::shared_ptr<CharacterPhysics> characterPhysics = std::make_shared<CharacterPhysics>();

		characterPhysics->Initialize(info, mPhysics);
		mCharacterPhysicsContainer.insert(std::make_pair(info.id, characterPhysics));

		std::shared_ptr<CollisionData> collisionData = std::make_shared<CollisionData>();
		collisionData->myId = info.id;
		collisionData->myLayerNumber = info.layerNumber;
		mCollisionDataConttainer.insert(std::make_pair(info.id, collisionData));

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
