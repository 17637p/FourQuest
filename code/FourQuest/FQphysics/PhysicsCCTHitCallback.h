#pragma once

#include <PxPhysicsAPI.h>

namespace fq::physics
{
	class PhysicsCCTHitCallback : public physx::PxControllerFilterCallback
	{
	public:
		PhysicsCCTHitCallback();
		virtual ~PhysicsCCTHitCallback();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="hit"></param>
		virtual bool filter(const physx::PxController& a, const physx::PxController& b) override;

	private:

	};
}