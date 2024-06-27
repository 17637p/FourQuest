#pragma once

#include "FQCommonPhysics.h"
#include <PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class RigidBody
	{
	public:
		/// <summary>
		/// 콜라이더 타입과 ID, CollisionMatrix의 layerNumber를 입력 받습니다.
		/// </summary>
		/// <param name="colliderType"> 콜라이더 타입 </param>
		/// <param name="id"> 게임엔진 액터 아이디 </param>
		/// <param name="layerNumber"> 콜리전 매트릭스의 레이어 넘버 </param>
		RigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber);
		virtual ~RigidBody();


		inline const unsigned int GetID() const;
		inline const unsigned int GetLayerNumber() const;
		inline const EColliderType& GetColliderType() const;
		inline const float& GetRadius() const;
		inline const float& GetHalfHeight() const;
		inline const physx::PxVec3& GetExtent() const;
		inline const DirectX::SimpleMath::Vector3& GetScale() const;
		inline void SetRadius(const float& radius);
		inline void SetHalfHeight(const float& halfHeight);
		inline void SetExtent(const float& x, const float& y, const float& z);
		inline void SetScale(const DirectX::SimpleMath::Vector3& scale);

		virtual void SetConvertScale(const DirectX::SimpleMath::Vector3& scale, physx::PxPhysics* physics, int* collisionMatrix) abstract;

	protected:
		void updateShapeGeometry(physx::PxRigidActor* actor, const physx::PxGeometry& newGeometry, physx::PxPhysics* physics, physx::PxMaterial* material, int* collisionMatrix);

	protected:
		unsigned int mID;
		unsigned int mLayerNumber;
		EColliderType mColliderType;

		DirectX::SimpleMath::Vector3 mScale;
		float mRadius;
		float mHalfHeight;
		physx::PxVec3 mExtent;
	};

	const unsigned int RigidBody::GetID() const
	{
		return mID;
	}
	const unsigned int RigidBody::GetLayerNumber() const
	{
		return mLayerNumber;
	}
	const EColliderType& RigidBody::GetColliderType() const
	{
		return mColliderType;
	}
	const float& RigidBody::GetRadius() const
	{
		return mRadius;
	}
	const float& RigidBody::GetHalfHeight() const
	{
		return mHalfHeight;
	}
	const physx::PxVec3& RigidBody::GetExtent() const
	{
		return mExtent;
	}
	const DirectX::SimpleMath::Vector3& RigidBody::GetScale() const
	{
		return mScale;
	}
	void RigidBody::SetRadius(const float& radius)
	{
		mRadius = radius;
	}
	void RigidBody::SetHalfHeight(const float& halfHeight)
	{
		mHalfHeight = halfHeight;
	}
	void RigidBody::SetExtent(const float& x, const float& y, const float& z)
	{
		mExtent.x = x;
		mExtent.y = y;
		mExtent.z = z;
	}
	void RigidBody::SetScale(const DirectX::SimpleMath::Vector3& scale)
	{
		mScale = scale;
	}
}