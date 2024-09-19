#include "DebugData.h"

#include "EngineDataConverter.h"
#include "DynamicRigidBody.h"
#include "StaticRigidBody.h"

namespace fq::physics
{
	DebugData::DebugData()
		: mDebugPolygon()
		, mDebugIndices()
		, mDebugVertices()
		, mDebugHeightField()
	{
	}

	DebugData::~DebugData()
	{
		mDebugPolygon.clear();
		mDebugIndices.clear();
		mDebugVertices.clear();
		mDebugHeightField.clear();
	}

	void DebugData::UpdateDebugData(std::shared_ptr<RigidBody> body)
	{
		using namespace std;

		mDebugPolygon.clear();
		mDebugIndices.clear();
		mDebugVertices.clear();
		mDebugHeightField.clear();

		std::shared_ptr<DynamicRigidBody> dynamicBody = std::dynamic_pointer_cast<DynamicRigidBody>(body);
		if (dynamicBody)
		{
			physx::PxShape* shape;
			physx::PxRigidActor* actor = dynamicBody->GetPxRigidDynamic();
			actor->getShapes(&shape, 1);

			if (shape != nullptr && shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
			{
				shared_ptr<vector<vector<DirectX::SimpleMath::Vector3>>> polygon = make_shared<vector<	vector<DirectX::SimpleMath::Vector3>>>();
				extractDebugConvexMesh(actor, shape, *polygon.get());

				mDebugPolygon.insert(std::make_pair(dynamicBody->GetID(), polygon));
			}
			else if (shape != nullptr && shape->getGeometry().getType() == physx::PxGeometryType::eTRIANGLEMESH)
			{
				std::vector<unsigned int> indices;
				std::vector<DirectX::SimpleMath::Vector3> vertices;

				extractDebugTriangleMesh(actor, shape, indices, vertices);

				mDebugIndices.insert(std::make_pair(body->GetID(), indices));
				mDebugVertices.insert(std::make_pair(body->GetID(), vertices));
			}
			else if (shape != nullptr && shape->getGeometry().getType() == physx::PxGeometryType::eHEIGHTFIELD)
			{
				std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>> line;
				extractDebugHeightField(actor, shape, line);

				mDebugHeightField.insert(std::make_pair(dynamicBody->GetID(), line));
			}
		}
		std::shared_ptr<StaticRigidBody> staticBody = dynamic_pointer_cast<StaticRigidBody>(body);
		if (staticBody)
		{
			physx::PxShape* shape;
			physx::PxRigidActor* actor = staticBody->GetPxRigidStatic();
			actor->getShapes(&shape, 1);

			if (shape != nullptr && shape->getGeometry().getType() == physx::PxGeometryType::eCONVEXMESH)
			{
				shared_ptr<vector<vector<DirectX::SimpleMath::Vector3>>> polygon = make_shared<vector<vector<DirectX::SimpleMath::Vector3>>>();
				extractDebugConvexMesh(actor, shape, *polygon.get());

				mDebugPolygon.insert(std::make_pair(dynamicBody->GetID(), polygon));
			}
			else if (shape != nullptr && shape->getGeometry().getType() == physx::PxGeometryType::eTRIANGLEMESH)
			{
				std::vector<unsigned int> indices;
				std::vector<DirectX::SimpleMath::Vector3> vertices;

				extractDebugTriangleMesh(actor, shape, indices, vertices);

				mDebugIndices.insert(std::make_pair(body->GetID(), indices));
				mDebugVertices.insert(std::make_pair(body->GetID(), vertices));
			}
			else if (shape != nullptr && shape->getGeometry().getType() == physx::PxGeometryType::eHEIGHTFIELD)
			{
				std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>> line;
				extractDebugHeightField(actor, shape, line);

				mDebugHeightField.insert(std::make_pair(staticBody->GetID(), line));
			}
		}
	}

	void DebugData::extractDebugConvexMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<std::vector<DirectX::SimpleMath::Vector3>>& debugPolygon)
	{
		using namespace DirectX::SimpleMath;
		using namespace std;

		const physx::PxConvexMeshGeometry& convexMeshGeometry = static_cast<const physx::PxConvexMeshGeometry&>(shape->getGeometry());

		Matrix dxMatrix;
		Matrix matrix;
		vector<vector<Vector3>> polygon;
		CopyPxTransformToDirectXMatrix(body->getGlobalPose(), dxMatrix);

		// PxConvexMesh에서 정점 및 인덱스 정보 얻기
		const physx::PxVec3* convexMeshVertices = convexMeshGeometry.convexMesh->getVertices();
		const physx::PxU32 vertexCount = convexMeshGeometry.convexMesh->getNbVertices();

		const physx::PxU8* convexMeshIndices = convexMeshGeometry.convexMesh->getIndexBuffer();
		const physx::PxU32 PolygonCount = convexMeshGeometry.convexMesh->getNbPolygons();
		polygon.reserve(PolygonCount);

		for (int i = 0; i < PolygonCount - 1; i++)
		{
			physx::PxHullPolygon pxPolygon;
			convexMeshGeometry.convexMesh->getPolygonData(i, pxPolygon);
			int vertexTotalNumber = pxPolygon.mNbVerts;
			int startIndexNumber = pxPolygon.mIndexBase;

			vector<Vector3> vertices;
			vertices.reserve(vertexTotalNumber);

			for (int j = 0; j < vertexTotalNumber; j++)
			{
				Vector3 vertex;
				vertex.x = -convexMeshVertices[convexMeshIndices[startIndexNumber + j]].x;
				vertex.y = -convexMeshVertices[convexMeshIndices[startIndexNumber + j]].y;
				vertex.z = -convexMeshVertices[convexMeshIndices[startIndexNumber + j]].z;

				vertex = Vector3::Transform(vertex, dxMatrix);

				vertices.push_back(vertex);
			}

			debugPolygon.push_back(vertices);
		}
	}

	void DebugData::extractDebugTriangleMesh(
		physx::PxRigidActor* body, 
		physx::PxShape* shape, 
		std::vector<unsigned int>& indices, 
		std::vector<DirectX::SimpleMath::Vector3>& vertices)
	{
		const physx::PxTriangleMeshGeometry& triangleGeometry = static_cast<const physx::PxTriangleMeshGeometry&>(shape->getGeometry());

		DirectX::SimpleMath::Matrix dxMatrix;
		CopyPxTransformToDirectXMatrix(body->getGlobalPose(), dxMatrix);

		const physx::PxVec3* meshVertices = triangleGeometry.triangleMesh->getVertices();
		const void* meshindices = triangleGeometry.triangleMesh->getTriangles();

		if (triangleGeometry.triangleMesh->getTriangleMeshFlags() && physx::PxTriangleMeshFlag::e16_BIT_INDICES)
		{
			const unsigned short* indices16Bits = static_cast<const unsigned short*>(meshindices);

			for (int i = 0; i < triangleGeometry.triangleMesh->getNbTriangles() * 3; i += 3)
			{
				unsigned int index1 = indices16Bits[i];
				unsigned int index2 = indices16Bits[i + 1];
				unsigned int index3 = indices16Bits[i + 2];

				indices.push_back(index1);
				indices.push_back(index2);
				indices.push_back(index3);
			}
		}
		else
		{
			const unsigned int* indices32Bits = static_cast<const unsigned int*>(meshindices);

			for (int i = 0; i < triangleGeometry.triangleMesh->getNbTriangles() * 3; i += 3)
			{
				unsigned int index1 = indices32Bits[i];
				unsigned int index2 = indices32Bits[i + 1];
				unsigned int index3 = indices32Bits[i + 2];

				indices.push_back(index1);
				indices.push_back(index2);
				indices.push_back(index3);
			}
		}

		for (int i = 0; i < triangleGeometry.triangleMesh->getNbVertices(); i++)
		{
			DirectX::SimpleMath::Vector3 vertex;
			vertex.x = -meshVertices[i].x;
			vertex.y = -meshVertices[i].y;
			vertex.z = -meshVertices[i].z;

			vertices.push_back(vertex);
		}
	}

	void DebugData::extractDebugHeightField(
		physx::PxRigidActor* body, 
		physx::PxShape* shape, 
		std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>>& line)
	{
		const physx::PxHeightFieldGeometry& heightFieldGeometry = static_cast<const physx::PxHeightFieldGeometry&>(shape->getGeometry());
		physx::PxHeightField* heightField = heightFieldGeometry.heightField;

		DirectX::SimpleMath::Matrix dxMatrix;
		CopyPxTransformToDirectXMatrix(body->getGlobalPose(), dxMatrix);

		unsigned int colsNumber = heightFieldGeometry.heightField->getNbColumns();
		unsigned int rowsNumber = heightFieldGeometry.heightField->getNbRows();

		physx::PxTransform transform = body->getGlobalPose();

		for (int i = 0; i < rowsNumber; i++)
		{
			for (int j = 0; j < colsNumber; j++)
			{
				if (j + 1 != colsNumber)
				{
					DirectX::SimpleMath::Vector3 line1;
					DirectX::SimpleMath::Vector3 line2;

					line1.x = transform.p.x + heightFieldGeometry.columnScale * i;
					line1.y = transform.p.y + heightFieldGeometry.heightField->getHeight(i, j) * heightFieldGeometry.heightScale;
					line1.z = - transform.p.z - heightFieldGeometry.rowScale * j;

					line2.x = transform.p.x + heightFieldGeometry.columnScale * (i);
					line2.y = transform.p.y + heightFieldGeometry.heightField->getHeight(i, j + 1) * heightFieldGeometry.heightScale;
					line2.z = - transform.p.z - heightFieldGeometry.rowScale * (j + 1);

					line.push_back(std::make_pair(line1, line2));
				}
				if (i + 1 != rowsNumber)
				{
					DirectX::SimpleMath::Vector3 line1;
					DirectX::SimpleMath::Vector3 line2;

					line1.x = transform.p.x + heightFieldGeometry.columnScale * i;
					line1.y = transform.p.y + heightFieldGeometry.heightField->getHeight(i, j) * heightFieldGeometry.heightScale;
					line1.z = -transform.p.z - heightFieldGeometry.rowScale * j;

					line2.x = transform.p.x + heightFieldGeometry.columnScale * (i + 1);
					line2.y = transform.p.y + heightFieldGeometry.heightField->getHeight(i + 1, j) * heightFieldGeometry.heightScale;
					line2.z = -transform.p.z - heightFieldGeometry.rowScale * (j);

					line.push_back(std::make_pair(line1, line2));
				}
			}
		}
	}
}
