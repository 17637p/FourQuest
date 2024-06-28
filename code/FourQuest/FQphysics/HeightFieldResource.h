#pragma once

#include "ResourceBase.h"

#include <PxPhysicsAPI.h>

namespace fq::physics
{
	class HeightFieldResource : public ResourceBase
	{
	public:
		HeightFieldResource(physx::PxPhysics* physics, const int* height, const unsigned int& numCols, const unsigned int& numRows);
		virtual ~HeightFieldResource();

		inline physx::PxHeightField* GetHeightField();

	private:
		physx::PxHeightField* mHeightField;
	};

	physx::PxHeightField* HeightFieldResource::GetHeightField()
	{
		return mHeightField;
	}
}