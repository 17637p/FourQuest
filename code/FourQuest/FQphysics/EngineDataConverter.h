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