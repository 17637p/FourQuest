#include "HeightFieldResource.h"

#include "PhysicsCookingMeshTool.h"

namespace fq::physics
{
	HeightFieldResource::HeightFieldResource(physx::PxPhysics* physics, const int* height, const unsigned int& numCols, const unsigned int& numRows)
		: ResourceBase(EResourceType::HEIGHT_FIELD)
	{
		mHeightField = PhysicsCookingMeshTool::CookingHeightField(physics, height, numCols, numRows);
	}

	HeightFieldResource::~HeightFieldResource()
	{
		PX_RELEASE(mHeightField);
	}
}
