#include "PhysicsCookingMeshTool.h"

namespace fq::physics
{
	physx::PxConvexMesh* PhysicsCookingMeshTool::CookingConvexMesh(physx::PxPhysics* physics, DirectX::SimpleMath::Vector3* vertices, int vertexSize, int polygonLimit)
	{
		// 컨벡스 메시 생성
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

	physx::PxTriangleMesh* PhysicsCookingMeshTool::CookingTriangleMesh(
		physx::PxPhysics* physics,
		const DirectX::SimpleMath::Vector3* vertices,
		const unsigned int& vertexSize,
		const unsigned int* indices,
		const unsigned int& indexSize)
	{
		physx::PxTolerancesScale scale;
		physx::PxCookingParams params(scale);

		// GPU 충돌 검사
		params.buildGPUData = true;
		// 메쉬 청소 사용 안 함 - 개발 구성에 대해 메쉬 유효성 검사 수행
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		// 에지 사전 계산 사용 안 함, 각 삼각형에 대해 에지 설정, 연락처 생성 속도 저하
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		// lower hierarchy for internal mesh
		//params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = vertexSize;
		meshDesc.points.stride = sizeof(DirectX::SimpleMath::Vector3);
		meshDesc.points.data = vertices;

		meshDesc.triangles.count = indexSize / 3;
		meshDesc.triangles.stride = sizeof(unsigned int) * 3;
		meshDesc.triangles.data = indices;

#ifdef _DEBUG
		// 메쉬 청소를 하지 않고 조리하기 전에 메쉬의 유효성을 확인해야 합니다
		bool res = PxValidateTriangleMesh(params, meshDesc);
		PX_ASSERT(res);
#endif

		physx::PxTriangleMesh* aTriangleMesh = PxCreateTriangleMesh(params, meshDesc,
			physics->getPhysicsInsertionCallback());

		return aTriangleMesh;
	}

	physx::PxHeightField* PhysicsCookingMeshTool::CookingHeightField(
		physx::PxPhysics* physics, 
		const int* height, 
		const unsigned int& numCols,
		const unsigned int& numRows)
	{
		// Height Field 데이터 준비
		std::vector<physx::PxHeightFieldSample> samples(numRows * numCols);

		// 샘플 배열 초기화 (높이 값 설정)
		for (physx::PxU32 i = 1; i < numRows; ++i)
		{
			for (physx::PxU32 j = 1; j < numCols; ++j)
			{
				samples[(numRows - i) * numCols - j].height = -height[(numRows - i) * numCols - j];
				samples[(numRows - i) * numCols - j].setTessFlag();
			}
		}

		// Height Field Desc 생성
		physx::PxHeightFieldDesc hfDesc;
		hfDesc.format = physx::PxHeightFieldFormat::eS16_TM;
		hfDesc.nbRows = numRows;
		hfDesc.nbColumns = numCols;
		hfDesc.samples.data = samples.data();
		hfDesc.samples.stride = sizeof(physx::PxHeightFieldSample);

		// Cooking을 사용하여 Height Field 생성
		physx::PxHeightField* heightField = PxCreateHeightField(hfDesc, physics->getPhysicsInsertionCallback());
		if (!heightField) {
			// 오류 처리
			return nullptr;
		}

		return heightField;
	}
}