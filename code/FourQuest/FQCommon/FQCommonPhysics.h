#pragma once
#include "FQCommon.h"

namespace fq::physics
{
	enum class EColliderType
	{
		TRIGGER,
		COLLISION
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
		unsigned int id;
		unsigned int otherId;
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
		float staticFriction;						// 정적 마찰 계수
		float dynamicFriction;						// 동적 마찰 계수
		float restitution;							// 복원 계수
		float density;								// 밀도
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
		DirectX::SimpleMath::Vector3* vertices;
		int vertexSize;
		int vertexLimit;								// 컨벡스 메시에 생성할 버텍스 최대 수
	};
}