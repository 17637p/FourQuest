#include <cassert>
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
		if (shape == nullptr || mFilterData == nullptr)
		{
			return PxQueryHitType::eNONE;
		}

		auto shapeCount = actor->getNbShapes();
		assert(shapeCount >= 1);

		if (shapeCount == 0)
		{
			return PxQueryHitType::eNONE;
		}

		PxShape* curShape;
		actor->getShapes(&curShape, 1);
		auto type = curShape->getConcreteType();

		physx::PxFilterData data = curShape->getSimulationFilterData();

		if (data.word0 > 32)
			return PxQueryHitType::eNONE;

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
