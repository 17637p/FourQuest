#pragma once

#include <physx\PxPhysicsAPI.h>

namespace fq::physics
{
	class PhysicsCCTHitCallback : physx::PxUserControllerHitReport
	{
	public:
		PhysicsCCTHitCallback();
		virtual ~PhysicsCCTHitCallback();

		virtual void onShapeHit(const physx::PxControllerShapeHit& hit) override;
		virtual void onControllerHit(const physx::PxControllersHit& hit) override;
		virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;

	private:

	};

}