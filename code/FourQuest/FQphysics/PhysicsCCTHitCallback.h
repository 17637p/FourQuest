#pragma once

#include <physx\PxPhysicsAPI.h>

namespace fq::physics
{
	class PhysicsCCTHitCallback : physx::PxUserControllerHitReport
	{
	public:
		PhysicsCCTHitCallback();
		virtual ~PhysicsCCTHitCallback();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="hit"></param>
		virtual void onShapeHit(const physx::PxControllerShapeHit& hit) override;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="hit"></param>
		virtual void onControllerHit(const physx::PxControllersHit& hit) override;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="hit"></param>
		virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;

	private:

	};
}