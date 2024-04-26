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
void DebugCapsule(physx::PxRigidActor* _body, physx::PxShape* _shape);

/// <summary>
/// Box ������ ���� RigidBody ����� ������ ����
/// </summary>
/// <param name="_body"> ������ �ٵ� </param>
/// <param name="_shape"> ���� </param>
void DebugBox(physx::PxRigidActor* _body, physx::PxShape* _shape);

/// <summary>
/// Sphere ������ ���� RigidBody ����� ������ ����
/// </summary>
/// <param name="_body"> ������ �ٵ� </param>
/// <param name="_shape"> ���� </param>
void DebugSphere(physx::PxRigidActor* _body, physx::PxShape* _shape);

/// <summary>
/// ConvexMesh ������ ���� RigidBody ����� ������ ����
/// </summary>
/// <param name="_body"> ������ �ٵ� </param>
/// <param name="_shape"> ���� </param>
void DebugConvexMesh(physx::PxRigidActor* _body, physx::PxShape* _shape);

