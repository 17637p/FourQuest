#pragma once
#include "FQCommon.h"

#include <string>

namespace fq::physics
{
#pragma region EnumClass
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
	/// ���� ��
	/// </summary>
	enum class EArticulationAxis
	{
		SWING_1,
		SWING_2,
		TWIST
	};

	/// <summary>
	/// ���� ���
	/// </summary>
	enum class EArticulationMotion
	{
		LOCKED = 0,
		LIMITED = 1,
		FREE = 2
	};
#pragma endregion

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
		std::vector<DirectX::SimpleMath::Vector3> ContectPoints;
		bool isDead = false;
	};

#pragma region GetSetData
	/// <summary>
	/// ���� �������� ������ �ٵ� �������� �ְ� �޴� GetSet ����ü
	/// </summary>
	struct RigidBodyGetSetData
	{
		DirectX::SimpleMath::Matrix transform = {};				// Ʈ������ �����Դϴ�.
		DirectX::SimpleMath::Vector3 linearVelocity = {};		// ���� 0�̸� StaticBody �Դϴ�.
		DirectX::SimpleMath::Vector3 angularVelocity = {};		// ���� 0�̸� StaticBody �Դϴ�.
	};

	struct CharacterControllerGetSetData
	{
		DirectX::SimpleMath::Vector3 position;					// ĳ���� ��Ʈ�ѷ��� ��ġ
	};

	struct CharacterMovementGetSetData
	{
		DirectX::SimpleMath::Vector3 velocity;					// ĳ���� ��Ʈ�ѷ��� x,y,z�� �ӵ�
		bool isFall;											// ĳ���Ͱ� �������� �ִ��� üũ ����
	};
#pragma endregion

#pragma region Collider
	constexpr unsigned int unregisterID = 0;

	/// <summary>
	/// �ݶ��̴� : ���� �浹 ó���� ���� ������Ʈ ����
	/// </summary>
	struct ColliderInfo
	{
		unsigned int id = unregisterID;
		unsigned int layerNumber = 0;
		fq::common::Transform collisionTransform;
		float staticFriction = 1.f;									// ���� ���� ���
		float dynamicFriction = 1.f;								// ���� ���� ���
		float restitution = 1.f;									// ���� ���
		float density = 1.f;										// �е�
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
		DirectX::SimpleMath::Vector3* vertices;					// �� ���ؽ�
		int vertexSize;											// �� ���ؽ� ������
		unsigned char convexPolygonLimit = 4;					// ������ �޽ÿ� ������ ������ �ִ� �� ( �ּ� : 4�� �̻�, �ִ� 256�� �̸� )
	};
#pragma endregion

#pragma region CharacterController
	/// <summary>
	/// ĳ���� �����Ʈ : ĳ���� �̵��� ���õ� ������
	/// </summary>
	struct CharacterMovementInfo
	{
		float maxSpeed = 5.f;									// �̵� �ִ� �ӵ� : ĳ���Ͱ� ������ �� �ִ� �ִ� �ӵ�
		float acceleration = 100.f;								// ���ӵ� : ĳ���Ͱ� �Է� ���� ���� �� �̵� ���ӵ�
		float staticFriction = 0.4f;							// ���� ���� ��� : ĳ���Ͱ� �̵� �� ������ �� ĳ���Ͱ� �޴� ������ ( 0.0f ~ 1.f )
		float dynamicFriction = 0.1f;							// ���� ���� ��� : �̵� �߿� ĳ���Ͱ� �޴� ������ ( 0.0f ~ 1.f )
		float jumpSpeed = 5.f;									// ����(y��) �ӵ�
		float jumpXZAcceleration = 10.f;						// ���� �߿� �̵�(XZ��) ���ӵ� ��
		float jumpXZDeceleration = 0.1f;						// ���� �߿� �̵�(XZ��) ���� �� ( 0.0 ~ 1.0 )
		float gravityWeight = 9.8f;								// �⺻ �߷� ���� �� �� ������ ����ġ�� �� �ְ� ���� �� ���� �ٸ��� ������ �� �ֽ��ϴ�.
	};

	/// <summary>
	/// ĳ���� ��Ʈ�ѷ� : ĳ���� ��Ʈ�ѷ� ������ ���� ��Ʈ�ѷ� ������
	/// </summary>
	struct CharacterControllerInfo
	{
		unsigned int id = unregisterID;							// ĳ���� ��Ʈ�ѷ� ���̵�
		unsigned int layerNumber = 0;							// �浹 ��Ʈ���� ���̾� �ѹ�

		DirectX::SimpleMath::Vector3 position{ 0.f, 0.f, 0.f };	// ĳ���� ��Ʈ�ѷ��� ��ġ�ϴ� ó�� ���� ��ġ
		float height = 30.f;									// ĳ���� ��Ʈ�ѷ�(ĸ��)�� ����
		float radius = 20.f;									// ĳ���� ��Ʈ�ѷ�(ĸ��)�� ������
		float stepOffset = 0.0f;								// ĳ���� ��Ʈ�ѷ��� ������ �� �ִ� 
		float slopeLimit = 0.7f;								// ĳ���Ͱ� �ɾ� �ö� �� �ִ� �ִ� ����
		float contactOffset = 0.1f;								// ��Ʈ�ѷ��� ���� ������ : ��ġ ���е� ������ �����ϱ� ���� ����մϴ�.
	};
#pragma endregion

#pragma region CharacterPhysics
	struct JointAxisInfo
	{
		EArticulationMotion motion;						// ��� ����
		float limitsLow;								// ���� ( Limit�� �� ȸ�� �Ʒ� ���� : 0.0 ~ 1.0 ) 
		float limitsHigh;								// ���� ( Limit�� �� ȸ�� �� ���� : 0.0 ~ 1.0 )

	};

	struct CharacterJointInfo
	{
		JointAxisInfo jointSwing1AxisInfo;				// Swing1( X���� �߽����� �� ȸ�� )
		JointAxisInfo jointSwing2AxisInfo;				// Swing2( Y���� �߽����� �� ȸ�� )
		JointAxisInfo jointTwistAxisInfo;				// Twist( Z���� �߽����� �� ȸ�� )
		DirectX::SimpleMath::Matrix localTransform;		// ����Ʈ�� ���� ��ǥ
		float stiffness = 100.f;						// ���� : ������ ��ǥ ��ġ�� �̵��Ϸ��� ���� ũ��
		float damping = 10.f;							// ���� ��� : ��� ���� ���׷� ( ������ �����ϰ� �ε巴�� �����̵� �� �� �ְ� )
		float maxForce = 1000.f;						// �ִ� �� : ���� ����̺갡 ������ �� �ִ� �ִ� ��
	};

	struct CharacterLinkInfo
	{
		std::string boneName;							// �ش� ��(��ũ)�� �̸�
		std::string parentBoneName;						// �θ� ��(��ũ)�� �̸�
		float density;
		CharacterJointInfo characterJointInfo;			// ����Ʈ ����
		DirectX::SimpleMath::Matrix localTransform;		// ���� ��ǥ
	};

	struct CharacterPhysicsInfo
	{
		std::string modelPath;
		unsigned int id = unregisterID;					// ���̵�
		unsigned int layerNumber = 0;					// ���̾� �ѹ�
		DirectX::SimpleMath::Matrix worldTransform;		// ���� ��ǥ
		float staticFriction = 1.f;						// ���� ���� ���
		float dynamicFriction = 1.f;					// ���� ���� ���
		float restitution = 1.f;						// ���� ���
		float density = 1.f;							// �е�
	};
#pragma endregion
}