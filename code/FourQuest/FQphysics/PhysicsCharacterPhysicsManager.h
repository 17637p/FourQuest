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
		/// 파직스 매니저 초기화
		/// </summary>
		bool initialize(physx::PxPhysics* physics, physx::PxScene* scene, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager);

		/// <summary>
		/// 캐릭터 파직스 (관절) 추가
		/// </summary>
		bool CreateCharacterphysics(const CharacterPhysicsInfo& info);

		/// <summary>
		/// 가지고 있는 관절 중, 링크 및 조인트 추가
		/// </summary>
		template <typename ...Params>
		bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, int* collisionMatrix, Params... params);

		/// <summary>
		/// 물리 공간에 추가하여 CharacterPhysics를 시뮬레이션할 캐릭터 파직스
		/// </summary>
		bool SimulationCharacter(unsigned int id);

		/// <summary>
		/// 씬을 변경할 때 호출하는 함수
		/// </summary>
		bool ChangeScene();

		/// <summary>
		/// 캐릭터 파직스를 반환하는 함수
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