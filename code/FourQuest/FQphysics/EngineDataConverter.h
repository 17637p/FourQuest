#pragma once

#include <directxtk\SimpleMath.h>
#include <physx\PxPhysicsAPI.h>

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
/// Capsule ������ ���� RigidBody ����� ������ ����
/// </summary>
/// <param name="_body"> ������ �ٵ� </param>
/// <param name="_shape"> ���� </param>
void ExtractDebugCapsule(physx::PxRigidActor* body, physx::PxShape* shape);

/// <summary>
/// Box ������ ���� RigidBody ����� ������ ����
/// </summary>
/// <param name="_body"> ������ �ٵ� </param>
/// <param name="_shape"> ���� </param>
void ExtractDebugBox(physx::PxRigidActor* body, physx::PxShape* shape, DirectX::BoundingOrientedBox& myBox);

/// <summary>
/// Sphere ������ ���� RigidBody ����� ������ ����
/// </summary>
/// <param name="_body"> ������ �ٵ� </param>
/// <param name="_shape"> ���� </param>
void ExtractDebugSphere(physx::PxRigidActor* body, physx::PxShape* shape, DirectX::BoundingSphere& mySphere);

/// <summary>
/// ConvexMesh ������ ���� RigidBody ����� ������ ����
/// </summary>
/// <param name="_body"> ������ �ٵ� </param>
/// <param name="_shape"> ���� </param>
void ExtractDebugConvexMesh(physx::PxRigidActor* body, physx::PxShape* shape, std::vector<std::vector<DirectX::SimpleMath::Vector3>>& myPolygon);

