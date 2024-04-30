#pragma once

#include "RigidBody.h"

namespace fq::physics
{
	class StaticRigidBody : public RigidBody
	{
	public:
		/// <summary>
		/// 콜라이더 타입과 ID, CollisionMatrix의 layerNumber를 입력 받습니다.
		/// </summary>
		/// <param name="colliderType"> 콜라이더 타입 </param>
		/// <param name="id"> 게임엔진 액터 아이디 </param>
		/// <param name="layerNumber"> 콜리전 매트릭스의 레이어 넘버 </param>
		StaticRigidBody(fq::physics::EColliderType colliderType, unsigned int id, unsigned int layerNumber);
		virtual ~StaticRigidBody();

		/// <summary>
		/// 리지드 바디를 생성할 데이터를 받아서 형태가 있는 리지드 바디를 생성합니다.
		/// </summary>
		/// <param name="colliderInfo"> 콜라이더 타입 </param>
		/// <param name="shape"> 도형 </param>
		/// <param name="physics"> physx물리 </param>
		bool Initialize(ColliderInfo colliderInfo, physx::PxShape* shape, physx::PxPhysics* physics);

		/// <summary>
		/// 스태틱 리지드 바디를 반환합니다.
		/// </summary>
		inline physx::PxRigidStatic* GetPxRigidStatic();

	private:
		physx::PxRigidStatic* mRigidStatic;
	};

	physx::PxRigidStatic* StaticRigidBody::GetPxRigidStatic()
	{
		return mRigidStatic;
	}
}