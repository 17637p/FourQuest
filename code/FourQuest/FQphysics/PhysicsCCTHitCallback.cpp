#include "PhysicsCCTHitCallback.h"

namespace fq::physics
{
	PhysicsCCTHitCallback::PhysicsCCTHitCallback()
	{
	}

	PhysicsCCTHitCallback::~PhysicsCCTHitCallback()
	{
	}

	bool PhysicsCCTHitCallback::filter(const physx::PxController& a, const physx::PxController& b)
	{
		physx::PxShape* myShape;
		physx::PxShape* otherShape;
		a.getActor()->getShapes(&myShape, 1);
		b.getActor()->getShapes(&otherShape, 1);

		auto myFilterData = myShape->getSimulationFilterData();
		auto otherFilterData = otherShape->getSimulationFilterData();

		if ((((1 << myFilterData.word0) & otherFilterData.word1) > 0) && (((1 << otherFilterData.word0) & myFilterData.word1) > 0))
		{
			return true;
		}

		return false;
	}
}

