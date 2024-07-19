#include "EngineDataConverter.h"

#include <memory>

#pragma region ConvertCoordinates

// ������ ��ǥ��(PhysX)���� �޼� ��ǥ��(DirectX11)�� ��ȯ�ϱ�
void CopyPxTransformToDirectXMatrix(const physx::PxTransform& pxTransform, DirectX::SimpleMath::Matrix& dxMatrix)
{
	DirectX::SimpleMath::Vector3 translation = DirectX::SimpleMath::Vector3(pxTransform.p.x, pxTransform.p.y, -pxTransform.p.z);
	DirectX::SimpleMath::Quaternion rotation = DirectX::SimpleMath::Quaternion(pxTransform.q.x, pxTransform.q.y, pxTransform.q.z, pxTransform.q.w);

	// z�� ���� ����
	DirectX::XMVECTOR flippedZ = DirectX::XMVectorNegate(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
	rotation = DirectX::XMQuaternionMultiply(rotation, flippedZ);

	dxMatrix = DirectX::XMMatrixAffineTransformation(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), DirectX::XMVectorZero(), rotation, translation);
}

// �޼� ��ǥ��(DirectX11)���� ������ ��ǥ��(PhysX)�� ��ȯ�ϱ� ( z�� )
void CopyDirectXMatrixToPxTransform(const DirectX::SimpleMath::Matrix& dxMatrix, physx::PxTransform& pxTransform)
{
	DirectX::XMFLOAT4X4 dxMatrixData;
	DirectX::XMStoreFloat4x4(&dxMatrixData, dxMatrix);

	pxTransform.p.x = dxMatrixData._41;
	pxTransform.p.y = dxMatrixData._42;
	pxTransform.p.z = -dxMatrixData._43; // z�� ���� ����

	// ȸ�� �������� z�� ���� ���� ����
	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationMatrix(dxMatrix);
	DirectX::XMVECTOR flippedZ = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XM_PI);
	quaternion = DirectX::XMQuaternionMultiply(quaternion, flippedZ);

	pxTransform.q.x = DirectX::XMVectorGetX(quaternion);
	pxTransform.q.y = DirectX::XMVectorGetY(quaternion);
	pxTransform.q.z = DirectX::XMVectorGetZ(quaternion);
	pxTransform.q.w = DirectX::XMVectorGetW(quaternion);
}

// �޼� ��ǥ��(DirectX11)���� ������ ��ǥ��(PhysX)�� ��ȯ�ϱ� ( x, y�� )
void CopyDirectXMatrixToPxTransformXYZ(const DirectX::SimpleMath::Matrix& dxMatrix, physx::PxTransform& pxTransform)
{
	DirectX::XMFLOAT4X4 dxMatrixData;
	DirectX::XMStoreFloat4x4(&dxMatrixData, dxMatrix);

	// ��ġ ���� ��ȯ
	pxTransform.p.x = -dxMatrixData._41; // x�� ����
	pxTransform.p.y = -dxMatrixData._42; // y�� ����
	pxTransform.p.z = -dxMatrixData._43;  // z�� ����

	// ȸ�� ������ ��ȯ
	DirectX::XMMATRIX rotationMatrix = dxMatrix;

	// x��� y�� ���� ����
	rotationMatrix.r[0].m128_f32[0] = -rotationMatrix.r[0].m128_f32[0];
	rotationMatrix.r[0].m128_f32[1] = -rotationMatrix.r[0].m128_f32[1];
	rotationMatrix.r[0].m128_f32[2] = -rotationMatrix.r[0].m128_f32[2];

	rotationMatrix.r[1].m128_f32[0] = -rotationMatrix.r[1].m128_f32[0];
	rotationMatrix.r[1].m128_f32[1] = -rotationMatrix.r[1].m128_f32[1];
	rotationMatrix.r[1].m128_f32[2] = -rotationMatrix.r[1].m128_f32[2];

	rotationMatrix.r[2].m128_f32[0] = rotationMatrix.r[2].m128_f32[0];
	rotationMatrix.r[2].m128_f32[1] = rotationMatrix.r[2].m128_f32[1];
	rotationMatrix.r[2].m128_f32[2] = rotationMatrix.r[2].m128_f32[2];

	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);

	pxTransform.q.x = DirectX::XMVectorGetX(quaternion);
	pxTransform.q.y = DirectX::XMVectorGetY(quaternion);
	pxTransform.q.z = DirectX::XMVectorGetZ(quaternion);
	pxTransform.q.w = DirectX::XMVectorGetW(quaternion);
}

#pragma endregion

#pragma region DebugDataExtract

void ExtractDebugConvexMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<std::vector<DirectX::SimpleMath::Vector3>>& debugPolygon)
{
	using namespace DirectX::SimpleMath;
	using namespace std;

	const physx::PxConvexMeshGeometry& convexMeshGeometry = static_cast<const physx::PxConvexMeshGeometry&>(shape->getGeometry());

	Matrix dxMatrix;
	Matrix matrix;
	vector<vector<Vector3>> polygon;
	CopyPxTransformToDirectXMatrix(body->getGlobalPose(), dxMatrix);

	// PxConvexMesh���� ���� �� �ε��� ���� ���
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

#pragma endregion



void CopyPxVec3ToDxVec3(const physx::PxVec3& pxVector, DirectX::SimpleMath::Vector3& dxVector)
{
	dxVector.x = pxVector.x;
	dxVector.y = pxVector.y;
	dxVector.z = -pxVector.z;
}

void CopyDxVec3ToPxVec3(const DirectX::SimpleMath::Vector3& dxVector, physx::PxVec3& pxVector)
{
	pxVector.x = dxVector.x;
	pxVector.y = dxVector.y;
	pxVector.z = -dxVector.z;
}