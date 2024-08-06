#pragma once

#include <memory>
#include <string>
#include <PxPhysicsAPI.h>
#include <directxtk\SimpleMath.h>

#include "EngineDataConverter.h"
#include "FQCommonPhysics.h"
#include "CharacterLink.h"

namespace fq::physics
{
	class CharacterLink;
	class PhysicsCollisionDataManager;

	class CharacterPhysics
	{
	public:
		CharacterPhysics();
		~CharacterPhysics();

		/// <summary>
		/// 캐릭터 파직스(관절) 초기화 함수
		/// </summary>
		bool Initialize(const ArticulationInfo& info, physx::PxPhysics* physics, std::shared_ptr<CollisionData> collisionData, physx::PxScene* scene);

		/// <summary>
		/// 업데이트
		/// </summary>
		bool Update();

		/// <summary>
		/// 캐릭터 파직스(관절) 링크 추가 함수
		/// </summary>
		bool AddArticulationLink(const LinkInfo& info, int* collisionMatrix, const DirectX::SimpleMath::Vector3& extent);
		bool AddArticulationLink(const LinkInfo& info, int* collisionMatrix, const float& radius);
		bool AddArticulationLink(const LinkInfo& info, int* collisionMatrix, const float& halfHeight, const float& radius);
		bool AddArticulationLink(LinkInfo& info, int* collisionMatrix);

		/// <summary>
		/// 레이어 넘버 바꾸기
		/// </summary>
		bool ChangeLayerNumber(const int& newLayerNumber, int* collisionMatrix, std::shared_ptr<PhysicsCollisionDataManager> mPhysicsCollisionDataManager);

		inline const std::string& GetModelPath();
		inline const unsigned int GetID();
		inline const unsigned int GetLayerNumber();
		inline const bool GetIsRagdoll();
		inline const std::shared_ptr<CharacterLink> GetRootLink();
		inline const std::shared_ptr<CharacterLink> FindLink(std::string name);
		inline const DirectX::SimpleMath::Matrix& GetWorldTransform();
		inline physx::PxArticulationReducedCoordinate* GetPxArticulation();
		inline const physx::PxMaterial* GetMaterial();
		inline const std::unordered_map<std::string, std::shared_ptr<CharacterLink>>& GetLinkContainer();

		inline void SetWorldTransform(const DirectX::SimpleMath::Matrix& trnasform);
		inline void SetIsRagdoll(const bool& isRagdoll);
		bool SetLinkTransformUpdate(const std::string& name, const DirectX::SimpleMath::Matrix& boneWorldTransform);

	private:
		std::string  mModelPath;
		unsigned int mID;
		unsigned int mLayerNumber;
		bool mbIsRagdoll;

		std::shared_ptr<CharacterLink> mRootLink;
		std::shared_ptr<CollisionData> mCollisionData;
		std::unordered_map<std::string, std::shared_ptr<CharacterLink>> mLinkContainer;
		DirectX::SimpleMath::Matrix mWorldTransform;

	private:
		physx::PxScene* mScene;
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
	const bool CharacterPhysics::GetIsRagdoll()
	{
		return mbIsRagdoll;
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
	const std::unordered_map<std::string, std::shared_ptr<CharacterLink>>& CharacterPhysics::GetLinkContainer()
	{
		return mLinkContainer;
	}

	void CharacterPhysics::SetWorldTransform(const DirectX::SimpleMath::Matrix& trnasform)
	{
		mWorldTransform = trnasform;

		physx::PxTransform pxWorldTransform;
		CopyDirectXMatrixToPxTransform(mWorldTransform, pxWorldTransform);
		mPxArticulation->setRootGlobalPose(pxWorldTransform);
	}
	void CharacterPhysics::SetIsRagdoll(const bool& isRagdoll)
	{
		mbIsRagdoll = isRagdoll;
	}
	inline bool CharacterPhysics::SetLinkTransformUpdate(const std::string& name, const DirectX::SimpleMath::Matrix& boneWorldTransform)
	{
		auto link = mLinkContainer.find(name);

		link->second->SetWorldTransform(boneWorldTransform);

		physx::PxTransform pxTransform;
		CopyDirectXMatrixToPxTransform(mWorldTransform, pxTransform);
		mPxArticulation->setRootGlobalPose(pxTransform);

		return true;
	}
#pragma endregion
}