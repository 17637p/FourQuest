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
		/// �ݶ��̴� Ÿ�԰� ID, CollisionMatrix�� layerNumber�� �Է� �޽��ϴ�.
		/// </summary>
		/// <param name="colliderType"> �ݶ��̴� Ÿ�� </param>
		/// <param name="id"> ���ӿ��� ���� ���̵� </param>
		/// <param name="layerNumber"> �ݸ��� ��Ʈ������ ���̾� �ѹ� </param>
		RigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber);
		virtual ~RigidBody();

		inline const unsigned int GetID() const;
		inline const unsigned int GetLayerNumber() const;
		inline const EColliderType& GetColliderType() const;
		inline const float& GetRadius() const;
		inline const float& GetHalfHeight() const;
		inline const physx::PxVec3& GetExtent() const;
		inline void SetRadius(const float& radius);
		inline void SetHalfHeight(const float& halfHeight);
		inline void SetExtent(const float& x, const float& y, const float& z);

		virtual void SetScale(const DirectX::SimpleMath::Vector3& scale) abstract;

	protected:
		unsigned int mID;
		unsigned int mLayerNumber;
		EColliderType mColliderType;

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
}