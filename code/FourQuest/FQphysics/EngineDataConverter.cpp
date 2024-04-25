#include "EngineDataConverter.h"

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

// �޼� ��ǥ��(DirectX11)���� ������ ��ǥ��(PhysX)�� ��ȯ�ϱ�
void CopyDirectXMatrixToPxTransform(const DirectX::SimpleMath::Matrix& dxMatrix, physx::PxTransform& pxTransform)
{
	DirectX::XMFLOAT4X4 dxMatrixData;
	DirectX::XMStoreFloat4x4(&dxMatrixData, dxMatrix);

	pxTransform.p.x = dxMatrixData._41;
	pxTransform.p.y = dxMatrixData._42;
	pxTransform.p.z = -dxMatrixData._43; // z�� ���� ����

	// ȸ�� �������� z�� ���� ���� ����
	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationMatrix(dxMatrix);
	DirectX::XMVECTOR flippedZ = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XM_PI);
	quaternion = DirectX::XMQuaternionMultiply(quaternion, flippedZ);

	pxTransform.q.x = DirectX::XMVectorGetX(quaternion);
	pxTransform.q.y = DirectX::XMVectorGetY(quaternion);
	pxTransform.q.z = DirectX::XMVectorGetZ(quaternion);
	pxTransform.q.w = DirectX::XMVectorGetW(quaternion);
}