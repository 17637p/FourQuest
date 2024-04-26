#pragma once
#include "FQCommon.h"

namespace fq::physics
{
	/// <summary>
	/// 콜라이더 타입
	/// 트리거 : 충돌되지 않고 겹치면 오버랩 이벤트만 호출
	/// 콜리전 : 충돌되고 콜리전 이벤트도 호출
	/// </summary>
	enum class EColliderType
	{
		TRIGGER,
		COLLISION
	};
	
	/// <summary>
	/// 콜백 함수에 전달되는 콜리전 이벤트 형태
	/// </summary>
	enum class ECollisionEventType
	{
		ENTER_OVERLAP = 0,
		ON_OVERLAP,
		END_OVERLAP,
		ENTER_COLLISION,
		ON_COLLISION,
		END_COLLISION,
	};

	/// <summary>
	/// 물리 엔진 세팅 데이터 : 중력, 충돌 매트릭스
	/// </summary>
	struct PhysicsEngineInfo
	{
		DirectX::SimpleMath::Vector3 gravity;
		int collisionMatrix[16];
	};

	/// <summary>
	/// 콜리전 : 충돌 데이터
	/// </summary>
	struct CollisionData
	{
		unsigned int myId;
		unsigned int otherId;
		unsigned int myLayerNumber;
		unsigned int otherLayerNumber;
		DirectX::SimpleMath::Vector3 CollisionPoint;
	};

	/// <summary>
	/// 콜라이더 : 물리 충돌 처리를 위한 오브젝트 형태
	/// </summary>
	struct ColliderInfo
	{
		unsigned int id;
		unsigned int layerNumber;
		fq::common::Transform collisionTransform;
		float staticFriction;								// 정적 마찰 계수
		float dynamicFriction;								// 동적 마찰 계수
		float restitution;									// 복원 계수
		float density;										// 밀도
	};

	struct BoxColliderInfo
	{
		ColliderInfo colliderInfo;
		DirectX::SimpleMath::Vector3 boxExtent;
	};
	struct SphereColliderInfo
	{
		ColliderInfo colliderInfo;
		float raidus;
	};
	struct CapsuleColliderInfo
	{
		ColliderInfo colliderInfo;
		float raidus;
		float halfHeight;
	};
	struct ConvexMeshColliderInfo
	{
		ColliderInfo colliderInfo;
		DirectX::SimpleMath::Vector3* vertices;				// 모델 버텍스
		int vertexSize;										// 모델 버텍스 사이즈
		unsigned char convexVertexLimit;					// 컨벡스 메시에 생성할 버텍스 최대 수 ( 최소 : 4개 이상, 최대 256개 미만 )
		unsigned char convexPolygonLimit;					// 컨벡스 메시에 생성할 폴리곤 최대 수 ( 최소 : 4개 이상, 최대 256개 미만 )
	};
}