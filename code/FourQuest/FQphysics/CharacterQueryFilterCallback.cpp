#include "CharacterQueryFilterCallback.h"

#include <cassert>

namespace fq::physics
{
	CharacterQueryFilterCallback::CharacterQueryFilterCallback(std::shared_ptr<physx::PxFilterData> filterData)
		: mFilterData(filterData)
	{
	}

	PxQueryHitType::Enum CharacterQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
	{
		assert(shape != nullptr);

		if (shape == nullptr)
		{
			return PxQueryHitType::eNONE;
		}

		physx::PxFilterData data = shape->getSimulationFilterData();

		if ((((1 << mFilterData->word0) & data.word1) > 0) && (((1 << data.word0) & mFilterData->word1) > 0))
		{
			return PxQueryHitType::eBLOCK;
		}

		return PxQueryHitType::eNONE;
	}

	PxQueryHitType::Enum CharacterQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor)
	{
		physx::PxFilterData data = shape->getSimulationFilterData();

		if ((((1 << mFilterData->word0) & data.word1) > 0) && (((1 << data.word0) & mFilterData->word1) > 0))
		{
			return PxQueryHitType::eBLOCK;
		}

		return PxQueryHitType::eBLOCK;
	}
}
