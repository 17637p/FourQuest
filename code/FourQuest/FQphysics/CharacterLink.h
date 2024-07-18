#pragma once

#include <vector>
#include <memory>
#include <string>

#include <directxtk\SimpleMath.h>
#include <PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterJoint;

	class CharacterLink : public std::enable_shared_from_this<CharacterLink>
	{
	public:
		CharacterLink();
		~CharacterLink();

		/// <summary>
		/// 캐릭터 링크 초기화 함수
		/// </summary>
		bool Initialize(const LinkInfo& info, std::shared_ptr<CharacterLink> parentLink, physx::PxArticulationReducedCoordinate* pxArticulation);

		/// <summary>
		/// 캐릭터 링크 지오메트리 추가 함수
		/// </summary>
		physx::PxShape* CreateShape(const physx::PxMaterial* material, const DirectX::SimpleMath::Vector3& extent, std::shared_ptr<CollisionData> collisionData);
		physx::PxShape* CreateShape(const physx::PxMaterial* material, const float& radius, const float& halfHeight, std::shared_ptr<CollisionData> collisionData);
		physx::PxShape* CreateShape(const physx::PxMaterial* material, const float& radius, std::shared_ptr<CollisionData> collisionData);

		/// <summary>
		/// 레이어 넘버 바꾸기
		/// </summary>
		bool ChangeLayerNumber(const physx::PxFilterData& filterData, CollisionData* collisionData);

		inline physx::PxArticulationLink* GetPxLink();
		inline const std::string& GetName();
		inline const DirectX::SimpleMath::Matrix& GetLocalTransform();
		inline const std::shared_ptr<CharacterJoint> GetCharacterJoint();
		inline const std::weak_ptr<CharacterLink> GetParentLink();
		inline const std::vector<std::weak_ptr<CharacterLink>>& GetChildrenCharacterLink();
		inline const std::weak_ptr<CharacterLink> GetChildCharacterLink(std::string linkName);

	private:
		std::string mName;
		float mDensity;
		DirectX::SimpleMath::Matrix mLocalTransform;
		physx::PxTransform mPxLocalTransform;

		std::shared_ptr<CharacterJoint> mMyJoint;
		std::weak_ptr<CharacterLink> mParentLink;
		std::vector<std::weak_ptr<CharacterLink>> mMyChildrenLink;

	private:
		physx::PxArticulationLink* mPxLink;
	};

#pragma region GetSet
	physx::PxArticulationLink* CharacterLink::GetPxLink()
	{
		return mPxLink;
	}
	const std::string& CharacterLink::GetName()
	{
		return mName;
	}
	const DirectX::SimpleMath::Matrix& CharacterLink::GetLocalTransform()
	{
		return mLocalTransform;
	}
	const std::shared_ptr<CharacterJoint> CharacterLink::GetCharacterJoint()
	{
		return mMyJoint;
	}
	const std::weak_ptr<CharacterLink> CharacterLink::GetParentLink()
	{
		return mParentLink;
	}
	const std::vector<std::weak_ptr<CharacterLink>>& CharacterLink::GetChildrenCharacterLink()
	{
		return mMyChildrenLink;
	}
	const std::weak_ptr<CharacterLink> CharacterLink::GetChildCharacterLink(std::string linkName)
	{
		for (const auto& childLink : mMyChildrenLink)
		{
			if (childLink.lock()->GetName() == linkName)
			{
				return childLink;
			}
		}
	}
#pragma endregion
}