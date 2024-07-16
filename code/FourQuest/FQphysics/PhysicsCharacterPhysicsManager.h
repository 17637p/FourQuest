#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <PxPhysicsAPI.h>

#include "FQCommonPhysics.h"
#include "CharacterPhysics.h"
#include "PhysicsCollisionDataManager.h"

namespace fq::physics
{
	class PhysicsCollisionDataManager;

	class PhysicsCharacterPhysicsManager
	{
	public:
		PhysicsCharacterPhysicsManager();
		~PhysicsCharacterPhysicsManager();
		
		/// <summary>
		/// ������ �Ŵ��� �ʱ�ȭ
		/// </summary>
		bool initialize(physx::PxPhysics* physics, physx::PxScene* scene, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager);

		/// <summary>
		/// ĳ���� ������ (����) �߰�
		/// </summary>
		bool CreateCharacterphysics(const ArticulationInfo& info);

		/// <summary>
		/// ĳ���� ������ ����
		/// </summary>
		bool RemoveArticulation(const unsigned int& id);

		/// <summary>
		/// ������ �ִ� ���� ��, ��ũ �� ����Ʈ �߰�
		/// </summary>
		template <typename ...Params>
		bool AddArticulationLink(unsigned int id, const LinkInfo& info, int* collisionMatrix, Params... params);

		/// <summary>
		/// ���� ������ �� ȣ���ϴ� �Լ�
		/// </summary>
		bool ChangeScene();

		/// <summary>
		/// ĳ���� �������� ��ȯ�ϴ� �Լ�
		/// </summary>
		inline std::shared_ptr<CharacterPhysics> GetCharacterPhysics(unsigned int id);

		void GetArticulationData(const unsigned int& id, ArticulationGetData& articulationData);
		void SetArticulationData(const unsigned int& id, const ArticulationSetData& articulationData, int* collisionMatrix);

	private:
		std::weak_ptr<PhysicsCollisionDataManager> mCollisionDataManager;

		physx::PxPhysics* mPhysics;
		physx::PxScene* mScene;

		std::unordered_map<unsigned int, std::shared_ptr<CharacterPhysics>> mCharacterPhysicsContainer;
	};

	template<typename ...Params>
	bool PhysicsCharacterPhysicsManager::AddArticulationLink(unsigned int id, const LinkInfo& info, int* collisionMatrix, Params ...params)
	{
		assert(mCharacterPhysicsContainer.find(id) != mCharacterPhysicsContainer.end());
		std::shared_ptr<CharacterPhysics> characterPhysics = mCharacterPhysicsContainer.find(id)->second;

		characterPhysics->AddArticulationLink(info, collisionMatrix, params...);
		return true;
	}

#pragma region GetSet
	std::shared_ptr<CharacterPhysics> PhysicsCharacterPhysicsManager::GetCharacterPhysics(unsigned int id)
	{
		return mCharacterPhysicsContainer.find(id)->second;
	}

#pragma endregion
}