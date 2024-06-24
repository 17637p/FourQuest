#include "PhysicsCookingMeshTool.h"

namespace fq::physics
{
	physx::PxConvexMesh* PhysicsCookingMeshTool::CookingConvexMesh(physx::PxPhysics* physics, DirectX::SimpleMath::Vector3* vertices, int vertexSize, int polygonLimit)
	{
		// ������ �޽� ����
		physx::PxConvexMeshDesc convexdesc;
		convexdesc.points.count = vertexSize;
		convexdesc.points.stride = sizeof(physx::PxVec3);
		convexdesc.vertexLimit = 255;
		convexdesc.points.data = (void*)vertices;
		convexdesc.polygonLimit = polygonLimit;
		convexdesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxTolerancesScale scale;
		physx::PxCookingParams params(scale);
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		params.buildGPUData = true;

		physx::PxDefaultMemoryOutputStream buf;
		physx::PxConvexMeshCookingResult::Enum result;

		assert(PxCookConvexMesh(params, convexdesc, buf, &result));

		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* convexMesh = physics->createConvexMesh(input);

		return convexMesh;
	}


	physx::PxTriangleMesh* PhysicsCookingMeshTool::CookingTriangleMesh(physx::PxPhysics* physics, DirectX::SimpleMath::Vector3* vertices, unsigned int vertexSize, unsigned int* indices, unsigned int indexSize)
	{
		physx::PxTolerancesScale scale;
		physx::PxCookingParams params(scale);
		// �޽� û�� ��� �� �� - ���� ������ ���� �޽� ��ȿ�� �˻� ����
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		// ���� ���� ��� ��� �� ��, �� �ﰢ���� ���� ���� ����, ����ó ���� �ӵ� ����
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		// lower hierarchy for internal mesh
		//params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = vertexSize;
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.points.data = vertices;

		meshDesc.triangles.count = indexSize;
		meshDesc.triangles.stride = 3 * sizeof(unsigned int);
		meshDesc.triangles.data = indices;

#ifdef _DEBUG
		// �޽� û�Ҹ� ���� �ʰ� �����ϱ� ���� �޽��� ��ȿ���� Ȯ���ؾ� �մϴ�
		bool res = PxValidateTriangleMesh(params, meshDesc);
		PX_ASSERT(res);
#endif

		physx::PxTriangleMesh* aTriangleMesh = PxCreateTriangleMesh(params, meshDesc,
			physics->getPhysicsInsertionCallback());
	}
}