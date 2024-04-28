#include "PhysicsSimulationEventCallback.h"

#include <functional>
#include <iostream>

namespace fq::physics
{
	PhysicsSimulationEventCallback::PhysicsSimulationEventCallback()
	{
	}

	PhysicsSimulationEventCallback::~PhysicsSimulationEventCallback()
	{
	}

	void PhysicsSimulationEventCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
	{
	}

	void PhysicsSimulationEventCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
	{
	}

	void PhysicsSimulationEventCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
	{
	}

	void PhysicsSimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		/// ENTER_COLLISION 충돌 이벤트 실행
		if (pairs[0].events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
		{
			SettingCollisionData(pairHeader, pairs, ECollisionEventType::ENTER_COLLISION);
		}

		/// END_COLLISION 충돌 이벤트 실행
		else if (pairs[0].events & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
		{
			SettingCollisionData(pairHeader, pairs, ECollisionEventType::END_COLLISION);
		}

		/// ON_COLLSION 충돌 이벤트 실행
		else if (pairs[0].events & (physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
		{
			SettingCollisionData(pairHeader, pairs, ECollisionEventType::ON_COLLISION);
		}
	}

	void PhysicsSimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		/// ENTER_OVERLAP 충돌 이벤트 실행
		if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			SettingTriggerData(pairs, ECollisionEventType::ENTER_OVERLAP);
		}

		/// END_OVERLAP 충돌 이벤트 실행
		if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			SettingTriggerData(pairs, ECollisionEventType::END_OVERLAP);
		}
	}

	void PhysicsSimulationEventCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
	{
	}

#pragma region SettingCollisionData

	void PhysicsSimulationEventCallback::SettingCollisionData(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, const ECollisionEventType& eventType)
	{
		std::vector<physx::PxContactPairPoint> contactPoints;

		const physx::PxContactPair& cp = pairs[0];
		std::vector<DirectX::SimpleMath::Vector3> points;

		physx::PxU32 contactCount = cp.contactCount;
		contactPoints.resize(contactCount);
		points.resize(contactCount);
		cp.extractContacts(&contactPoints[0], contactCount);

		for (physx::PxU32 j = 0; j < contactCount; j++)
		{
			points[j].x = contactPoints[j].position.x;
			points[j].y = contactPoints[j].position.x;
			points[j].z = contactPoints[j].position.x;
		}

		CollisionData ActorData1;
		CollisionData ActorData2;
		CollisionData* myData = (CollisionData*)pairHeader.actors[0]->userData;
		CollisionData* otherData = (CollisionData*)pairHeader.actors[1]->userData;

		ActorData1.myId = myData->myId;
		ActorData1.otherId = otherData->myId;
		ActorData1.myLayerNumber = myData->myLayerNumber;
		ActorData1.otherLayerNumber = otherData->myLayerNumber;
		ActorData2.myId = otherData->myId;
		ActorData2.otherId = myData->myId;
		ActorData2.myLayerNumber = otherData->myLayerNumber;
		ActorData2.otherLayerNumber = myData->myLayerNumber;

		ActorData1.ContectPoints = points;
		ActorData2.ContectPoints = points;

		mFunction(ActorData1, eventType);
		mFunction(ActorData2, eventType);
	}

	void PhysicsSimulationEventCallback::SettingTriggerData(const physx::PxTriggerPair* pairs, const ECollisionEventType& eventType)
	{
		CollisionData Mydata;
		CollisionData Otherdata;
		CollisionData* TriggerActorData = (CollisionData*)pairs->triggerActor->userData;
		CollisionData* OtherActordata = (CollisionData*)pairs->triggerActor->userData;

		Mydata.myId = TriggerActorData->myId;
		Mydata.otherId = OtherActordata->myId;
		Mydata.myLayerNumber = TriggerActorData->myLayerNumber;
		Mydata.otherLayerNumber = OtherActordata->myLayerNumber;
		Otherdata.myId = OtherActordata->myId;
		Otherdata.otherId = TriggerActorData->myId;
		Otherdata.myLayerNumber = OtherActordata->myLayerNumber;
		Otherdata.otherLayerNumber = TriggerActorData->myLayerNumber;

		// 콜백 함수 실행
		mFunction(Mydata, eventType);
		mFunction(Otherdata, eventType);
	}

#pragma endregion

}