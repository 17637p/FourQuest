#pragma once

#include <vector>
#include <memory>
#include <string>

#include <directxtk\SimpleMath.h>
#include <physx\PxPhysicsAPI.h>

namespace fq::physics
{
	class CharacterJoint;

	class CharacterLink
	{
	public:
		CharacterLink();
		~CharacterLink();

		void Initialize();

		inline physx::PxArticulationLink* GetPxLink();
		inline const std::string GetName();
		inline const DirectX::SimpleMath::Matrix GetLocalTransform();
		inline const std::shared_ptr<CharacterJoint> GetCharacterJoint();
		inline const std::vector<std::shared_ptr<CharacterLink>>& GetChildrenCharacterLink();
		inline const std::shared_ptr<CharacterLink> GetChildCharacterLink(std::string linkName);

	private:
		std::string mName;
		DirectX::SimpleMath::Matrix mLocalTransform;

		std::shared_ptr<CharacterJoint> mMyJoint;
		std::vector<std::shared_ptr<CharacterLink>> mMyChildrenLink;

	private:
		physx::PxArticulationLink* mPxLink;
	};

#pragma region GetSet
	physx::PxArticulationLink* CharacterLink::GetPxLink()
	{
		return mPxLink;
	}
	const std::string CharacterLink::GetName()
	{
		return mName;
	}
	const DirectX::SimpleMath::Matrix CharacterLink::GetLocalTransform()
	{
		return mLocalTransform;
	}
	const std::shared_ptr<CharacterJoint> CharacterLink::GetCharacterJoint()
	{
		return mMyJoint;
	}
	const std::vector<std::shared_ptr<CharacterLink>>& CharacterLink::GetChildrenCharacterLink()
	{
		return mMyChildrenLink;
	}
	const std::shared_ptr<CharacterLink> CharacterLink::GetChildCharacterLink(std::string linkName)
	{
		for (const auto& childLink : mMyChildrenLink)
		{
			if (childLink->GetName() == linkName)
			{
				return childLink;
			}
		}
	}
#pragma endregion
}