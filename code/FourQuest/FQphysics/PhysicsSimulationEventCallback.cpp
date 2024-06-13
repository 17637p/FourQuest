#include "PhysicsSimulationEventCallback.h"

#include <functional>
#include <iostream>
#include <spdlog/spdlog.h>

#include "PhysicsCollisionDataManager.h"

namespace fq::physics
{
	PhysicsSimulationEventCallback::PhysicsSimulationEventCallback()
	{
	}

	PhysicsSimulationEventCallback::~PhysicsSimulationEventCallback()
	{
	}

	void PhysicsSimulationEventCallback::Initialize(std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager)
	{
		mCollisionDataManager = collisionDataManager;
	}

	void PhysicsSimulationEventCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
	{
	}

	void PhysicsSimulationEventCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
	{

		std::cout << "onWake" << std::endl;
	}

	void PhysicsSimulationEventCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
	{
		std::cout << "onSleep" << std::endl;
	}

	void PhysicsSimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		for (int i = 0; i < nbPairs; i++)
		{
			if (pairs[i].events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
			{
				SettingCollisionData(pairHeader, &pairs[i], ECollisionEventType::ENTER_COLLISION);
			}

			/// END_COLLISION 충돌 이벤트 실행
			else if (pairs[i].events & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
			{
				SettingCollisionData(pairHeader, &pairs[i], ECollisionEventType::END_COLLISION);
			}

			/// ON_COLLSION 충돌 이벤트 실행
			else if (pairs[i].events & (physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
			{
				SettingCollisionData(pairHeader, &pairs[i], ECollisionEventType::ON_COLLISION);
			}
		}
	}

	void PhysicsSimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		for (int i = 0; i < count; i++)
		{
			/// ENTER_OVERLAP 충돌 이벤트 실행
			if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				SettingTriggerData(&pairs[i], ECollisionEventType::ENTER_OVERLAP);
			}

			/// END_OVERLAP 충돌 이벤트 실행
			if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				SettingTriggerData(&pairs[i], ECollisionEventType::END_OVERLAP);
			}
		}
	}

	void PhysicsSimulationEventCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
	{
	}

	void PhysicsSimulationEventCallback::OnTrigger()
	{
		for (auto trigger : mTriggerContainer)
		{
			std::shared_ptr<CollisionData> TriggerActorData = mCollisionDataManager->FindCollisionData(trigger.first);

			if (TriggerActorData.get() == nullptr)
			{
				mTriggerContainer.erase(mTriggerContainer.find(trigger.first));
				return;
			}

			for (auto other : trigger.second)
			{
				std::shared_ptr<CollisionData> OtherActordata = mCollisionDataManager->FindCollisionData(other);
				CollisionData Mydata;
				CollisionData Otherdata;

				Mydata.myId = TriggerActorData->myId;
				Mydata.otherId = OtherActordata->myId;
				Mydata.myLayerNumber = TriggerActorData->myLayerNumber;
				Mydata.otherLayerNumber = OtherActordata->myLayerNumber;
				Otherdata.myId = OtherActordata->myId;
				Otherdata.otherId = TriggerActorData->myId;
				Otherdata.myLayerNumber = OtherActordata->myLayerNumber;
				Otherdata.otherLayerNumber = TriggerActorData->myLayerNumber;

				// 콜백 함수 실행
				mFunction(Mydata, ECollisionEventType::ON_OVERLAP);
				mFunction(Otherdata, ECollisionEventType::ON_OVERLAP);
			}
		}
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

		if (contactCount > 0)
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

		if (myData == nullptr || otherData == nullptr)
			return;

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

		if (eventType == ECollisionEventType::END_COLLISION && myData->isDead)
		{
			mCollisionDataManager->Remove(myData->myId);
		}
		if (eventType == ECollisionEventType::END_COLLISION && otherData->isDead)
		{
			mCollisionDataManager->Remove(otherData->myId);
		}
	}

	void PhysicsSimulationEventCallback::SettingTriggerData(const physx::PxTriggerPair* pairs, const ECollisionEventType& eventType)
	{
		CollisionData Mydata;
		CollisionData Otherdata;
		CollisionData* TriggerActorData = (CollisionData*)pairs->triggerActor->userData;
		CollisionData* OtherActordata = (CollisionData*)pairs->otherActor->userData;

		if (TriggerActorData == nullptr || OtherActordata == nullptr)
			return;

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

		CountTrigger(TriggerActorData->myId, OtherActordata->myId, eventType);

		if (eventType == ECollisionEventType::END_OVERLAP && TriggerActorData->isDead)
		{
			mCollisionDataManager->Remove(TriggerActorData->myId);
		}
		if (eventType == ECollisionEventType::END_OVERLAP && OtherActordata->isDead)
		{
			mCollisionDataManager->Remove(OtherActordata->myId);
		}
	}

	void PhysicsSimulationEventCallback::CountTrigger(unsigned int triggerID, unsigned int otherID, const ECollisionEventType& eventType)
	{
		if (eventType == ECollisionEventType::ENTER_OVERLAP)
		{
			auto iter = mTriggerContainer.find(triggerID);

			// 해당 트리거 아이디가 이미 다른 오브젝트와 충돌하고 있고 또 다른 오브젝트와 충돌했을 때 기존 값에 접근해서 충돌 오브젝트의 아이디 추가
			if (iter != mTriggerContainer.end())
			{
				iter->second.insert(otherID);
			}
			else // 해당 트리거 아이디가 아무와도 충돌하고 있지 않을 때 충돌되었다면 트리거 컨테이너에 추가
			{
				std::set<unsigned int> otherIDContainer;
				otherIDContainer.insert(otherID);
				mTriggerContainer.insert(std::make_pair(triggerID, otherIDContainer));
			}
		}
		else if (eventType == ECollisionEventType::END_OVERLAP)
		{
			auto triggerIter = mTriggerContainer.find(triggerID);
			auto otherIDIter = triggerIter->second.find(otherID);

			// 트리거 아이디와 부딪히고 있는 otherActorID 삭제
			if (otherIDIter != triggerIter->second.end())
				triggerIter->second.erase(otherIDIter);

			// 만약 해당 트리거 아이디가 부딪히고 있는 otherActorID가 없을 때 트리거 컨테이너에서 삭제
			if (triggerIter->second.empty())
				mTriggerContainer.erase(triggerIter);
		}
	}
#pragma endregion
}