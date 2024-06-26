#pragma once

#include <PxPhysicsAPI.h>

#include <memory>

namespace fq::physics
{
	using namespace physx;

	class CharacterQueryFilterCallback : public physx::PxQueryFilterCallback
	{
	public:
		CharacterQueryFilterCallback(std::shared_ptr<physx::PxFilterData> filterData);
		virtual ~CharacterQueryFilterCallback() {}

		virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
		virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) override;

		inline std::shared_ptr<physx::PxFilterData> GetFilterData();
		inline void SetFilterData(std::shared_ptr<physx::PxFilterData> newData);

	public:
		std::shared_ptr<physx::PxFilterData> mFilterData;
	};

	std::shared_ptr<physx::PxFilterData> CharacterQueryFilterCallback::GetFilterData()
	{
		return mFilterData;
	}
	void CharacterQueryFilterCallback::SetFilterData(std::shared_ptr<physx::PxFilterData> newData)
	{
		mFilterData = newData;
	}
}