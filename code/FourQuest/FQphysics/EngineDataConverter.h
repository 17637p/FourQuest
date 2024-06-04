#pragma once

#include <directxtk\SimpleMath.h>
#include <PxPhysicsAPI.h>

/// <summary>
/// ������ ��ǥ�� (PhysX)�� �޼� ��ǥ�� (DirectX)�� z�� ��ȯ�� ���� �������ݴϴ�.
/// </summary>
/// <param name="pxTransform"> ��ȯ�� ��Ʈ���� ���� ���� ������ �Ű� �����Դϴ�. </param>
/// <param name="dxMatrix"> ��ȯ�� ��Ʈ���� ���� ���� ���� �Ű� �����Դϴ�. </param>
void CopyPxTransformToDirectXMatrix(const physx::PxTransform& pxTransform, DirectX::SimpleMath::Matrix& dxMatrix);

/// <summary>
/// �޼� ��ǥ�� (DirectX)�� ������ ��ǥ�� (PhysX)�� z�� ��ȯ�� ���� �������ݴϴ�.
/// </summary>
/// <param name="dxMatrix"> ��ȯ�� ��Ʈ���� ���� ���� ������ �Ű� �����Դϴ�. </param>
/// <param name="pxTransform"> ��ȯ�� ��Ʈ���� ���� ���� ���� �Ű� �����Դϴ�. </param>
void CopyDirectXMatrixToPxTransform(const DirectX::SimpleMath::Matrix& dxMatrix, physx::PxTransform& pxTransform);

/// <summary>
/// ConvexMesh ������ ���� RigidBody ����� ������ ����
/// </summary>
/// <param name="_body"> ������ �ٵ� </param>
/// <param name="_shape"> ���� </param>
void ExtractDebugConvexMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<std::vector<DirectX::SimpleMath::Vector3>>& myPolygon);

void CopyPxVec3ToDxVec3(const physx::PxVec3& pxVector, DirectX::SimpleMath::Vector3& dxVector);
void CopyDxVec3ToPxVec3(const DirectX::SimpleMath::Vector3& dxVector, physx::PxVec3& pxVector);