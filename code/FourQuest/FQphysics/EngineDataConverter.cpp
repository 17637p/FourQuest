#include "EngineDataConverter.h"

#include <memory>

#pragma region ConvertCoordinates

// 오른손 좌표계(PhysX)에서 왼손 좌표계(DirectX11)로 변환하기
void CopyPxTransformToDirectXMatrix(const physx::PxTransform& pxTransform, DirectX::SimpleMath::Matrix& dxMatrix)
{
	DirectX::SimpleMath::Vector3 translation = DirectX::SimpleMath::Vector3(pxTransform.p.x, pxTransform.p.y, -pxTransform.p.z);
	DirectX::SimpleMath::Quaternion rotation = DirectX::SimpleMath::Quaternion(pxTransform.q.x, pxTransform.q.y, pxTransform.q.z, pxTransform.q.w);

	// z축 방향 반전
	DirectX::XMVECTOR flippedZ = DirectX::XMVectorNegate(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
	rotation = DirectX::XMQuaternionMultiply(rotation, flippedZ);

	dxMatrix = DirectX::XMMatrixAffineTransformation(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), DirectX::XMVectorZero(), rotation, translation);
}

// 왼손 좌표계(DirectX11)에서 오른손 좌표계(PhysX)로 변환하기
void CopyDirectXMatrixToPxTransform(const DirectX::SimpleMath::Matrix& dxMatrix, physx::PxTransform& pxTransform)
{
	DirectX::XMFLOAT4X4 dxMatrixData;
	DirectX::XMStoreFloat4x4(&dxMatrixData, dxMatrix);

	pxTransform.p.x = dxMatrixData._41;
	pxTransform.p.y = dxMatrixData._42;
	pxTransform.p.z = -dxMatrixData._43; // z축 방향 반전

	// 회전 정보에서 z축 방향 반전 적용
	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationMatrix(dxMatrix);
	DirectX::XMVECTOR flippedZ = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XM_PI);
	quaternion = DirectX::XMQuaternionMultiply(quaternion, flippedZ);

	pxTransform.q.x = DirectX::XMVectorGetX(quaternion);
	pxTransform.q.y = DirectX::XMVectorGetY(quaternion);
	pxTransform.q.z = DirectX::XMVectorGetZ(quaternion);
	pxTransform.q.w = DirectX::XMVectorGetW(quaternion);
}

#pragma endregion

#pragma region DebugDataExtract

void ExtractDebugCapsule(physx::PxRigidActor* _body, physx::PxShape* _shape)
{
	using namespace DirectX::SimpleMath;
	using namespace std;

	const physx::PxCapsuleGeometry& capsuleGeometry = static_cast<const physx::PxCapsuleGeometry&>(_shape->getGeometry());
	Matrix dxMatrix;
	CopyPxTransformToDirectXMatrix(_body->getGlobalPose(), dxMatrix);

	Vector3 scale;
	Vector3 translation;
	Quaternion quaternion;
	dxMatrix.Decompose(scale, quaternion, translation);

	Matrix Sphere0Matrix = dxMatrix * Matrix::CreateTranslation(capsuleGeometry.radius * dxMatrix.Right());
	Matrix Sphere1Matrix = dxMatrix * Matrix::CreateTranslation(-capsuleGeometry.radius * dxMatrix.Right());

	shared_ptr<DirectX::BoundingSphere> sphere0 = make_shared<DirectX::BoundingSphere>();
	shared_ptr<DirectX::BoundingSphere> sphere1 = make_shared<DirectX::BoundingSphere>();
	//shared_ptr<GraphicsEngine::DebugLineData> line1 = make_shared<GraphicsEngine::DebugLineData>();
	//shared_ptr<GraphicsEngine::DebugLineData> line2 = make_shared<GraphicsEngine::DebugLineData>();
	//shared_ptr<GraphicsEngine::DebugLineData> line3 = make_shared<GraphicsEngine::DebugLineData>();
	//shared_ptr<GraphicsEngine::DebugLineData> line4 = make_shared<GraphicsEngine::DebugLineData>();

	sphere0->Center = Sphere0Matrix.Translation();
	sphere1->Center = Sphere1Matrix.Translation();
	//line1->Pos0 = Sphere0Matrix.Translation() + Vector3(0.f, capsuleGeometry.radius, 0.f);
	//line1->Pos1 = Sphere1Matrix.Translation() + Vector3(0.f, capsuleGeometry.radius, 0.f);
	//line1->Color = Color(1.f, 1.f, 0.f, 1.f);
	//line2->Pos0 = Sphere0Matrix.Translation() + Vector3(0.f, -capsuleGeometry.radius, 0.f);
	//line2->Pos1 = Sphere1Matrix.Translation() + Vector3(0.f, -capsuleGeometry.radius, 0.f);
	//line2->Color = Color(1.f, 1.f, 0.f, 1.f);
	//line3->Pos0 = Sphere0Matrix.Translation() + Vector3(0.f, 0.f, capsuleGeometry.radius);
	//line3->Pos1 = Sphere1Matrix.Translation() + Vector3(0.f, 0.f, capsuleGeometry.radius);
	//line3->Color = Color(1.f, 1.f, 0.f, 1.f);
	//line4->Pos0 = Sphere0Matrix.Translation() + Vector3(0.f, 0.f, -capsuleGeometry.radius);
	//line4->Pos1 = Sphere1Matrix.Translation() + Vector3(0.f, 0.f, -capsuleGeometry.radius);
	//line4->Color = Color(1.f, 1.f, 0.f, 1.f);
	sphere0->Radius = capsuleGeometry.radius;
	sphere1->Radius = capsuleGeometry.radius;

	//RENDER->AddDebugLine(line1);
	//RENDER->AddDebugLine(line2);
	//RENDER->AddDebugLine(line3);
	//RENDER->AddDebugLine(line4);
	//RENDER->AddDebugSphere(sphere0);
	//RENDER->AddDebugSphere(sphere1);
}

void ExtractDebugBox(physx::PxRigidActor* body, physx::PxShape* shape, DirectX::BoundingOrientedBox& debugBox)
{
	using namespace DirectX::SimpleMath;
	using namespace std;

	DirectX::BoundingOrientedBox orientBox;
	const physx::PxBoxGeometry& boxGeometry = static_cast<const physx::PxBoxGeometry&>(shape->getGeometry());

	Matrix dxMatrix;
	CopyPxTransformToDirectXMatrix(body->getGlobalPose(), dxMatrix);

	Vector3 scale;
	Vector3 translation;
	Quaternion quaternion;
	dxMatrix.Decompose(scale, quaternion, translation);

	debugBox.Center = translation;
	debugBox.Extents = (const Vector3&)boxGeometry.halfExtents;
	debugBox.Orientation = quaternion;

	//RENDER->AddDebugBox(orientBox);
}

void ExtractDebugSphere(physx::PxRigidActor* body, physx::PxShape* shape, DirectX::BoundingSphere& debugSphere)
{
	using namespace DirectX::SimpleMath;
	using namespace std;

	DirectX::BoundingSphere sphere;
	const physx::PxSphereGeometry& sphereGeometry = static_cast<const physx::PxSphereGeometry&>(shape->getGeometry());

	Matrix dxMatrix;
	CopyPxTransformToDirectXMatrix(body->getGlobalPose(), dxMatrix);

	debugSphere.Center = dxMatrix.Translation();
	debugSphere.Radius = sphereGeometry.radius;

	//RENDER->AddDebugSphere(sphere);
}

void ExtractDebugConvexMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<std::vector<DirectX::SimpleMath::Vector3>>& debugPolygon)
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
			vertex.x = convexMeshVertices[convexMeshIndices[startIndexNumber + j]].x;
			vertex.y = convexMeshVertices[convexMeshIndices[startIndexNumber + j]].y;
			vertex.z = -convexMeshVertices[convexMeshIndices[startIndexNumber + j]].z;

			vertex = Vector3::Transform(vertex, dxMatrix);

			vertices.push_back(vertex);
		}

		debugPolygon.push_back(vertices);
		//RENDER->AddDebugPolygon(vertices);
	}
}

#pragma endregion
