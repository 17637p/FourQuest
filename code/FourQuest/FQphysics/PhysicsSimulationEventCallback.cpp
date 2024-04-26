#include "PhysicsSimulationEventCallback.h"
#include <functional>

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
		//std::vector<physx::PxContactPairPoint> contactPoints;

		//physx::PxTransform spherePose(physx::PxIdentity);
		//physx::PxU32 nextPairIndex = 0xfffffffff;

		//physx::PxContactPairExtraDataIterator iter(pairHeader.extraDataStream, pairHeader.extraDataStreamSize);
		//bool hasItemSet = iter.nextItemSet();
		//if (hasItemSet)
		//	nextPairIndex = iter.contactPairIndex;

		//for (physx::PxU32 i = 0; i < nbPairs; i++)
		//{
		//	const physx::PxContactPair& cp = pairs[i];

		//	if (cp.events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
		//	{
		//		physx::PxU32 contactCount = cp.contactCount;
		//		contactPoints.resize(contactCount);
		//		cp.extractContacts(&contactPoints[0], contactCount);

		//		for (physx::PxU32 j = 0; j < contactCount; j++)
		//		{
		//			CollisionPoints.push_back(contactPoints[j].position);

		//			std::cout << "==========================================================================" << std::endl;
		//			std::cout << "위치 : (" << contactPoints[j].position.x << ", " << contactPoints[j].position.y << ", " << contactPoints[j].position.z << ")" << std::endl;
		//			std::cout << "충돌 : (" << contactPoints[j].impulse.x << ", " << contactPoints[j].impulse.y << ", " << contactPoints[j].impulse.z << ")" << std::endl;
		//			std::cout << "==========================================================================" << std::endl;
		//		}
		//	}

		//	if (cp.events & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
		//	{
		//		std::cout << " 떨어집니다 " << std::endl;
		//	}
		//}

	}

	void PhysicsSimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		/// ENTER_OVERLAP 충돌 이벤트 실행
		if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
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
			mFunction(std::move(Mydata), ECollisionEventType::ENTER_OVERLAP);
			mFunction(std::move(Otherdata), ECollisionEventType::ENTER_OVERLAP);
		}

		/// END_OVERLAP 충돌 이벤트 실행
		if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
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
			mFunction(std::move(Mydata), ECollisionEventType::END_OVERLAP);
			mFunction(std::move(Otherdata), ECollisionEventType::END_OVERLAP);
		}
	}

	void PhysicsSimulationEventCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
	{
	}
}