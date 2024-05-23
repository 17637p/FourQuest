#include "ConvexMeshResource.h"

#include "PhysicsCookingMeshTool.h"

namespace fq::physics
{
	ConvexMeshResource::ConvexMeshResource(physx::PxPhysics* physics, DirectX::SimpleMath::Vector3* vertices, int vertexSize, int polygonLimit)
		: ResourceBase(EResourceType::CONVEX_MESH)
	{
		mConvexMesh = PhysicsCookingMeshTool::CookingConvexMesh(physics, vertices, vertexSize, polygonLimit);
	}

	ConvexMeshResource::~ConvexMeshResource()
	{
		PX_RELEASE(mConvexMesh);
	}
}
