#pragma once
#include "ResourceBase.h"

#include "physx/PxPhysicsAPI.h"

#include "FQCommonPhysics.h"

namespace fq::physics
{
    class ConvexMeshResource : public ResourceBase
    {
    public:
        ConvexMeshResource(ConvexMeshColliderInfo info);
        virtual ~ConvexMeshResource();

    private:
        physx::PxConvexMesh* mConvexMesh;
    };
}

