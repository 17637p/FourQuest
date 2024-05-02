#pragma once

#include <physx\PxPhysicsAPI.h>
#include "FQCommonPhysics.h"
#include <map>
#include <memory>

namespace fq::physics
{
	class RigidBody;

	class PhysicsSimulationEventCallback : public physx::PxSimulationEventCallback
	{
	public:
		PhysicsSimulationEventCallback();
		~PhysicsSimulationEventCallback();

		/// <summary>
		/// PhysX에서 충돌하면 발생되는 실행되는 함수들입니다.
		/// </summary>
		virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;
		virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override;
		virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override;
		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
		virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;

		/// <summary>
		/// 충돌 데이터를 세팅하여 콜백 함수를 호출합니다.
		/// </summary>
		/// <param name="pairHeader"> 두 쌍의 헤더 </param>
		/// <param name="pairs"> 두 쌍의 오브젝트 </param>
		/// <param name="eventType"> 충돌 이벤트 타입 </param>
		void SettingCollisionData(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, const ECollisionEventType& eventType);

		/// <summary>
		/// 충돌 데이터를 세팅하여 콜백 함수를 호출합니다.
		/// </summary>
		/// <param name="pairs"> 두 쌍의 오브젝트 </param>
		/// <param name="eventType"> 충돌 이벤트 타입 </param>
		void SettingTriggerData(const physx::PxTriggerPair* pairs, const ECollisionEventType& eventType);

		/// <summary>
		/// 콜백 함수를 등록합니다.
		/// </summary>
		/// <param name="func"> 등록할 콜백 함수 </param>
		inline void SetCallbackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func);

	private:
		std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> mFunction;
	};

	void PhysicsSimulationEventCallback::SetCallbackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func)
	{
		mFunction = func;
	}
}