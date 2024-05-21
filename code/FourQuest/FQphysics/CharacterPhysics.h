#pragma once

#include <memory>
#include <string>
#include <physx\PxPhysicsAPI.h>
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

		bool Initialize(const CharacterPhysicsInfo& info, physx::PxPhysics* physics);

		bool AddArticulationLink(const CharacterLinkInfo& info, const DirectX::SimpleMath::Vector3& extent, std::shared_ptr<CollisionData> collisionData);
		bool AddArticulationLink(const CharacterLinkInfo& info, const float& radius, std::shared_ptr<CollisionData> collisionData);
		bool AddArticulationLink(const CharacterLinkInfo& info, const float& halfHeight, const float& radius, std::shared_ptr<CollisionData> collisionData);

		inline const std::string& GetModelPath();
		inline const unsigned int GetID();
		inline const unsigned int GetLayerNumber();
		inline const std::shared_ptr<CharacterLink> GetRootLink();
		inline const std::shared_ptr<CharacterLink> FindLink(std::string name);
		inline const DirectX::SimpleMath::Matrix& GetWorldTransform();
		inline const physx::PxArticulationReducedCoordinate* GetPxArticulation();
		inline const physx::PxMaterial* GetMaterial();

	private:
		std::string  mModelPath;
		unsigned int mID;
		unsigned int mLayerNumber;

		std::shared_ptr<CharacterLink> mRootLink;
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
	const physx::PxArticulationReducedCoordinate* CharacterPhysics::GetPxArticulation()
	{
		return mPxArticulation;
	}
	const physx::PxMaterial* CharacterPhysics::GetMaterial()
	{
		return mMaterial;
	}
#pragma endregion
}