#include "ConvexMeshResource.h"


namespace fq::physics
{
	ConvexMeshResource::ConvexMeshResource(ConvexMeshColliderInfo info)
		: ResourceBase(EResourceType::CONVEX_MESH)
	{
	}

	ConvexMeshResource::~ConvexMeshResource()
	{
	}
}
