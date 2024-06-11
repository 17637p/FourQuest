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
		bool CreateCharacterphysics(const CharacterPhysicsInfo& info);

		/// <summary>
		/// ������ �ִ� ���� ��, ��ũ �� ����Ʈ �߰�
		/// </summary>
		template <typename ...Params>
		bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, int* collisionMatrix, Params... params);

		/// <summary>
		/// ���� ������ �߰��Ͽ� CharacterPhysics�� �ùķ��̼��� ĳ���� ������
		/// </summary>
		bool SimulationCharacter(unsigned int id);

		/// <summary>
		/// ���� ������ �� ȣ���ϴ� �Լ�
		/// </summary>
		bool ChangeScene();

		/// <summary>
		/// ĳ���� �������� ��ȯ�ϴ� �Լ�
		/// </summary>
		inline std::shared_ptr<CharacterPhysics> GetCharacterPhysics(unsigned int id);

	private:
		std::weak_ptr<PhysicsCollisionDataManager> mCollisionDataManager;

		physx::PxPhysics* mPhysics;
		physx::PxScene* mScene;

		std::unordered_map<unsigned int, std::shared_ptr<CharacterPhysics>> mCharacterPhysicsContainer;
	};

	template<typename ...Params>
	bool PhysicsCharacterPhysicsManager::AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, int* collisionMatrix, Params ...params)
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