#include "PhysicsSimulationEventCallback.h"

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
	}

	void PhysicsSimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
	}

	void PhysicsSimulationEventCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
	{
	}
}