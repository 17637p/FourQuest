#pragma once

#include <directxtk\SimpleMath.h>
#include <PxPhysicsAPI.h>

/// <summary>
/// 오른손 좌표계 (PhysX)를 왼손 좌표계 (DirectX)로 z축 전환을 통해 변경해줍니다.
/// </summary>
/// <param name="pxTransform"> 전환할 매트릭스 정보 값을 전달할 매개 변수입니다. </param>
/// <param name="dxMatrix"> 전환된 매트릭스 정보 값을 받을 매개 변수입니다. </param>
void CopyPxTransformToDirectXMatrix(const physx::PxTransform& pxTransform, DirectX::SimpleMath::Matrix& dxMatrix);

/// <summary>
/// 왼손 좌표계 (DirectX)를 오른손 좌표계 (PhysX)로 z축 전환을 통해 변경해줍니다.
/// </summary>
/// <param name="dxMatrix"> 전환할 매트릭스 정보 값을 전달할 매개 변수입니다. </param>
/// <param name="pxTransform"> 전환된 매트릭스 정보 값을 받을 매개 변수입니다. </param>
void CopyDirectXMatrixToPxTransform(const DirectX::SimpleMath::Matrix& dxMatrix, physx::PxTransform& pxTransform);



void CopyPxVec3ToDxVec3(const physx::PxVec3& pxVector, DirectX::SimpleMath::Vector3& dxVector);
void CopyDxVec3ToPxVec3(const DirectX::SimpleMath::Vector3& dxVector, physx::PxVec3& pxVector);