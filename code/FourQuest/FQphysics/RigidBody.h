#pragma once

#include "FQCommonPhysics.h"
#include <physx\PxPhysicsAPI.h>

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

		/// <summary>
		/// 게임 엔진에서 할당받은 아이디를 반환합니다.
		/// </summary>
		inline const unsigned int GetID() const;

		/// <summary>
		/// 게임 엔진에서 할당 받은 레이어 넘버를 반환합니다.
		/// </summary>
		inline const unsigned int GetLayerNumber() const;

		/// <summary>
		/// 리지드 바디의 콜라이더 타입을 반환합니다.
		/// </summary>
		inline const EColliderType& GetColliderType() const;

	private:
		unsigned int mID;
		unsigned int mLayerNumber;
		fq::physics::EColliderType	mColliderType;
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
}