#pragma once

#include <memory>
#include <string>
#include <PxPhysicsAPI.h>
#include <directxtk\SimpleMath.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterLink;

	class CharacterPhysics
	{
	public:
		CharacterPhysics();
		~CharacterPhysics();

		/// <summary>
		/// 캐릭터 파직스(관절) 초기화 함수
		/// </summary>
		bool Initialize(const CharacterPhysicsInfo& info, physx::PxPhysics* physics, std::shared_ptr<CollisionData> collisionData);

		/// <summary>
		/// 캐릭터 파직스(관절) 링크 추가 함수
		/// </summary>
		bool AddArticulationLink(const CharacterLinkInfo& info, int* collisionMatrix, const DirectX::SimpleMath::Vector3& extent);
		bool AddArticulationLink(const CharacterLinkInfo& info, int* collisionMatrix, const float& radius);
		bool AddArticulationLink(const CharacterLinkInfo& info, int* collisionMatrix, const float& halfHeight, const float& radius);

		inline const std::string& GetModelPath();
		inline const unsigned int GetID();
		inline const unsigned int GetLayerNumber();
		inline const std::shared_ptr<CharacterLink> GetRootLink();
		inline const std::shared_ptr<CharacterLink> FindLink(std::string name);
		inline const DirectX::SimpleMath::Matrix& GetWorldTransform();
		inline physx::PxArticulationReducedCoordinate* GetPxArticulation();
		inline const physx::PxMaterial* GetMaterial();

	private:
		std::string  mModelPath;
		unsigned int mID;
		unsigned int mLayerNumber;

		std::shared_ptr<CharacterLink> mRootLink;
		std::shared_ptr<CollisionData> mCollisionData;
		std::unordered_map<std::string, std::shared_ptr<CharacterLink>> mLinkContainer;
		DirectX::SimpleMath::Matrix mWorldTransform;

	private:
		physx::PxArticulationReducedCoordinate* mPxArticulation;
		physx::PxMaterial* mMaterial;
	};


#pragma region GetSet
	const std::string& CharacterPhysics::GetModelPath()
	{
		return mModelPath;
	}
	const unsigned int CharacterPhysics::GetID()
	{
		return mID;
	}
	const unsigned int CharacterPhysics::GetLayerNumber()
	{
		return mLayerNumber;
	}
	const std::shared_ptr<CharacterLink> CharacterPhysics::GetRootLink()
	{
		return mRootLink;
	}
	const std::shared_ptr<CharacterLink> CharacterPhysics::FindLink(std::string name)
	{
		return mLinkContainer.find(name)->second;
	}
	const DirectX::SimpleMath::Matrix& CharacterPhysics::GetWorldTransform()
	{
		return mWorldTransform;
	}
	physx::PxArticulationReducedCoordinate* CharacterPhysics::GetPxArticulation()
	{
		return mPxArticulation;
	}
	const physx::PxMaterial* CharacterPhysics::GetMaterial()
	{
		return mMaterial;
	}
#pragma endregion
}