#pragma once

#include <directxtk\SimpleMath.h>
#include <physx\PxPhysicsAPI.h>

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

/// <summary>
/// Capsule 도형을 가진 RigidBody 디버그 데이터 추출
/// </summary>
/// <param name="_body"> 리지드 바디 </param>
/// <param name="_shape"> 도형 </param>
void DebugCapsule(physx::PxRigidActor* _body, physx::PxShape* _shape);

/// <summary>
/// Box 도형을 가진 RigidBody 디버그 데이터 추출
/// </summary>
/// <param name="_body"> 리지드 바디 </param>
/// <param name="_shape"> 도형 </param>
void DebugBox(physx::PxRigidActor* _body, physx::PxShape* _shape);

/// <summary>
/// Sphere 도형을 가진 RigidBody 디버그 데이터 추출
/// </summary>
/// <param name="_body"> 리지드 바디 </param>
/// <param name="_shape"> 도형 </param>
void DebugSphere(physx::PxRigidActor* _body, physx::PxShape* _shape);

/// <summary>
/// ConvexMesh 도형을 가진 RigidBody 디버그 데이터 추출
/// </summary>
/// <param name="_body"> 리지드 바디 </param>
/// <param name="_shape"> 도형 </param>
void DebugConvexMesh(physx::PxRigidActor* _body, physx::PxShape* _shape);

