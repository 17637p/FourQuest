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
		
		/// <summary>
		/// 파직스 매니저 초기화
		/// </summary>
		bool initialize(physx::PxPhysics* physics, physx::PxScene* scene);

		/// <summary>
		/// 캐릭터 파직스 (관절) 추가
		/// </summary>
		bool CreateCharacterphysics(const CharacterPhysicsInfo& info);

		/// <summary>
		/// 가지고 있는 관절 중, 링크 및 조인트 추가
		/// </summary>
		bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, int* collisionMatrix, const DirectX::SimpleMath::Vector3& extent);
		bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, int* collisionMatrix, const float& radius);
		bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, int* collisionMatrix, const float& halfHeight, const float& radius);

		/// <summary>
		/// 물리 공간에 추가하여 CharacterPhysics를 시뮬레이션할 캐릭터 파직스
		/// </summary>
		bool SimulationCharacter(unsigned int id);

		/// <summary>
		/// 캐릭터 파직스를 반환하는 함수
		/// </summary>
		inline std::shared_ptr<CharacterPhysics> GetCharacterPhysics(unsigned int id);

	private:
		physx::PxPhysics* mPhysics;
		physx::PxScene* mScene;

		std::unordered_map<unsigned int, std::shared_ptr<CharacterPhysics>> mCharacterPhysicsContainer;
		std::unordered_map<unsigned int, std::shared_ptr<CollisionData>>	mCollisionDataConttainer;
	};

#pragma region GetSet
	std::shared_ptr<CharacterPhysics> PhysicsCharacterPhysicsManager::GetCharacterPhysics(unsigned int id)
	{
		return mCharacterPhysicsContainer.find(id)->second;
	}
#pragma endregion
}