#pragma once
#include "FQCommon.h"

namespace fq::physics
{
	/// <summary>
	/// �ݶ��̴� Ÿ��
	/// Ʈ���� : �浹���� �ʰ� ��ġ�� ������ �̺�Ʈ�� ȣ��
	/// �ݸ��� : �浹�ǰ� �ݸ��� �̺�Ʈ�� ȣ��
	/// </summary>
	enum class EColliderType
	{
		TRIGGER,
		COLLISION
	};
	
	/// <summary>
	/// �ݹ� �Լ��� ���޵Ǵ� �ݸ��� �̺�Ʈ ����
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
		unsigned int myId;
		unsigned int otherId;
		unsigned int myLayerNumber;
		unsigned int otherLayerNumber;
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
		float staticFriction;								// ���� ���� ���
		float dynamicFriction;								// ���� ���� ���
		float restitution;									// ���� ���
		float density;										// �е�
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
		DirectX::SimpleMath::Vector3* vertices;				// �� ���ؽ�
		int vertexSize;										// �� ���ؽ� ������
		unsigned char convexVertexLimit;					// ������ �޽ÿ� ������ ���ؽ� �ִ� �� ( �ּ� : 4�� �̻�, �ִ� 256�� �̸� )
		unsigned char convexPolygonLimit;					// ������ �޽ÿ� ������ ������ �ִ� �� ( �ּ� : 4�� �̻�, �ִ� 256�� �̸� )
	};
}