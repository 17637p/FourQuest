#pragma once
#include <PxQueryFiltering.h>

namespace fq::physics
{
	/// <summary>
	/// Raycast 쿼리용 필터 
	/// </summary>
	class RaycastQueryFileter : public physx::PxQueryFilterCallback
	{
	public:
		physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData& filterData
			, const physx::PxShape* shape
			, const physx::PxRigidActor* actor
			, physx::PxHitFlags& queryFlags) override;

		physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData
			, const physx::PxQueryHit& hit
			, const physx::PxShape* shape
			, const physx::PxRigidActor* actor) override;

		virtual ~RaycastQueryFileter() {}
	};	


}