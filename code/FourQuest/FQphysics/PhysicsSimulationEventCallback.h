#pragma once

#include <PxPhysicsAPI.h>
#include "FQCommonPhysics.h"
#include <map>
#include <set>
#include <memory>

namespace fq::physics
{
	class RigidBody;
	class PhysicsCollisionDataManager;

	class PhysicsSimulationEventCallback : public physx::PxSimulationEventCallback
	{
	public:
		PhysicsSimulationEventCallback();
		~PhysicsSimulationEventCallback();

		/// <summary>
		/// 
		/// </summary>
		void Initialize(std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager);

		/// <summary>
		/// PhysX���� �浹�ϸ� �߻��Ǵ� ����Ǵ� �Լ����Դϴ�.
		/// </summary>
		virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;
		virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override;
		virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override;
		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
		virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;

		/// <summary>
		/// 
		/// </summary>
		void OnTrigger();

		/// <summary>
		/// �浹 �����͸� �����Ͽ� �ݹ� �Լ��� ȣ���մϴ�.
		/// </summary>
		/// <param name="pairHeader"> �� ���� ��� </param>
		/// <param name="pairs"> �� ���� ������Ʈ </param>
		/// <param name="eventType"> �浹 �̺�Ʈ Ÿ�� </param>
		void SettingCollisionData(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, const ECollisionEventType& eventType);

		/// <summary>
		/// �浹 �����͸� �����Ͽ� �ݹ� �Լ��� ȣ���մϴ�.
		/// </summary>
		/// <param name="pairs"> �� ���� ������Ʈ </param>
		/// <param name="eventType"> �浹 �̺�Ʈ Ÿ�� </param>
		void SettingTriggerData(const physx::PxTriggerPair* pairs, const ECollisionEventType& eventType);

		/// <summary>
		/// OnTrigger �浹 �̺�Ʈ�� �����Ű�� ���� TriggerObject�� �浹�ϰ� �ִ� ������Ʈ�� ī�����ϴ� �Լ��Դϴ�.
		/// </summary>
		/// <param name="triggerID"> Ʈ���� ���̵� </param>
		/// <param name="otherID"> �ش� Ʈ���ſ� �浹�ϰ� �ִ� ������Ʈ ���̵�</param>
		/// <param name="eventType"> �̺�Ʈ Ÿ�� </param>
		void CountTrigger(unsigned int triggerID, unsigned int otherID, const ECollisionEventType& eventType);

		/// <summary>
		/// �ݹ� �Լ��� ����մϴ�.
		/// </summary>
		/// <param name="func"> ����� �ݹ� �Լ� </param>
		inline void SetCallbackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func);

	private:
		std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> mFunction;
		std::shared_ptr<PhysicsCollisionDataManager> mCollisionDataManager;

		std::unordered_map<unsigned int, std::set<unsigned int>> mTriggerContainer;
	};

	void PhysicsSimulationEventCallback::SetCallbackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func)
	{
		mFunction = func;
	}
}