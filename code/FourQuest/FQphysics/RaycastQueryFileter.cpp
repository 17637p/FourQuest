#include "RaycastQueryFileter.h"

#include <spdlog/spdlog.h>
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

physx::PxQueryHitType::Enum fq::physics::RaycastQueryFileter::preFilter(
	const physx::PxFilterData& filterData
	, const physx::PxShape* shape
	, const physx::PxRigidActor* actor
	, physx::PxHitFlags& queryFlags)
{
	auto data = shape->getSimulationFilterData();

	if (filterData.word1 & (1 << data.word0))
	{
		return physx::PxQueryHitType::eTOUCH;
	}

	return physx::PxQueryHitType::eNONE;
}

physx::PxQueryHitType::Enum fq::physics::RaycastQueryFileter::postFilter(
	const physx::PxFilterData& filterData
	, const physx::PxQueryHit& hit
	, const physx::PxShape* shape
	, const physx::PxRigidActor* actor)
{
	auto data = shape->getSimulationFilterData();

	if (filterData.word1 & (1 << data.word0))
	{
		return physx::PxQueryHitType::eTOUCH;
	}

	return physx::PxQueryHitType::eNONE;
}
