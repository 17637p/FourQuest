#pragma once
#include "ResourceBase.h"

#include "physx/PxPhysicsAPI.h"

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class MaterialResource : public ResourceBase
	{
	public:
		MaterialResource();
		virtual ~MaterialResource();

	private:
		physx::PxMaterial* mMaterial;
	};
}

