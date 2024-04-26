#include "PhysicsCookingMeshTool.h"

namespace fq::physics
{
	PhysicsCookingMeshTool::PhysicsCookingMeshTool()
		: mPhysics()
		, mConvexMeshMap{}
	{
	}

	PhysicsCookingMeshTool::~PhysicsCookingMeshTool()
	{
	}

	bool PhysicsCookingMeshTool::Initialize(physx::PxPhysics* physics)
	{
		if (physics != nullptr)
		{
			mPhysics = physics;
			return true;
		}

		return false;
	}

	physx::PxConvexMesh* PhysicsCookingMeshTool::CookingConvexMesh(DirectX::SimpleMath::Vector3* vertices, int vertexSize, int vertexLimit)
	{
		physx::PxConvexMesh* convexMesh = FindConvexMesh(vertices);

		if (!convexMesh)
		{
			// 컨벡스 메시 생성
			physx::PxConvexMeshDesc convexdesc;
			convexdesc.points.count = vertexSize;
			convexdesc.points.stride = sizeof(physx::PxVec3);
			convexdesc.vertexLimit = vertexLimit;
			convexdesc.points.data = vertices;
			convexdesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

			physx::PxTolerancesScale scale;
			physx::PxCookingParams params(scale);

			physx::PxDefaultMemoryOutputStream buf;
			physx::PxConvexMeshCookingResult::Enum result;
			assert(PxCookConvexMesh(params, convexdesc, buf, &result));

			physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
			physx::PxConvexMesh* convexMesh = mPhysics->createConvexMesh(input);
		}

		return convexMesh;
	}

}