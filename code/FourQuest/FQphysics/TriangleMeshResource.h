#pragma once

#include "ResourceBase.h"

#include <PxPhysicsAPI.h>
#include <directxtk\SimpleMath.h>

namespace fq::physics
{
	class TriangleMeshResource : public ResourceBase
	{
	public:
		TriangleMeshResource(
			physx::PxPhysics* physics,
			const DirectX::SimpleMath::Vector3* vertices,
			const unsigned int& vertexSize,
			const unsigned int* indices,
			const unsigned int& indexSize);
		virtual ~TriangleMeshResource();

		inline physx::PxTriangleMesh* GetTriangleMesh();

	private:
		physx::PxTriangleMesh* mTriangleMesh;
	};

	physx::PxTriangleMesh* TriangleMeshResource::GetTriangleMesh()
	{
		return mTriangleMesh;
	}
}