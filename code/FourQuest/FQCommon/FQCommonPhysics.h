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
	/// ���� ���� ���� ������ : �߷�, �浹 ��Ʈ����
	/// </summary>
	struct PhysicsEngineInfo
	{
		DirectX::SimpleMath::Vector3 gravity;
		int collisionMatrix[16];
	};

	/// <summary>
	/// �ݸ��� : �浹 ������
	/// </summary>
	struct CollisionData
	{
		unsigned int id;
		unsigned int otherId;
		DirectX::SimpleMath::Vector3 CollisionPoint;
	};

	/// <summary>
	/// �ݶ��̴� : ���� �浹 ó���� ���� ������Ʈ ����
	/// </summary>
	struct ColliderInfo
	{
		unsigned int id;
		unsigned int layerNumber;
		fq::common::Transform collisionTransform;
		float staticFriction;						// ���� ���� ���
		float dynamicFriction;						// ���� ���� ���
		float restitution;							// ���� ���
		float density;								// �е�
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
		int vertexLimit;								// ������ �޽ÿ� ������ ���ؽ� �ִ� ��
	};
}