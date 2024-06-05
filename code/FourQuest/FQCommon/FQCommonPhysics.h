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

	struct RayCastData
	{
		unsigned int hitSize = 0;
		unsigned int myId;
		unsigned int myLayerNumber;
		std::vector<unsigned int> id;
		std::vector<unsigned int> layerNumber;
		std::vector<DirectX::SimpleMath::Vector3> contectPoints;
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
		DirectX::SimpleMath::Vector3 position = {};				// ĳ���� ��Ʈ�ѷ��� ��ġ
		DirectX::SimpleMath::Vector3 scale = {1.f,1.f,1.f};				// ĳ���� ��Ʈ�ѷ��� ��ġ
		DirectX::SimpleMath::Quaternion rotation = {};				// ĳ���� ��Ʈ�ѷ��� ��ġ
	};

	struct CharacterMovementGetSetData
	{
		DirectX::SimpleMath::Vector3 velocity = {};				// ĳ���� ��Ʈ�ѷ��� x,y,z�� �ӵ�
		bool isFall;											// ĳ���Ͱ� �������� �ִ��� üũ ����
	};
#pragma endregion

#pragma region Resource
	struct ConvexMeshResoureceInfo
	{
		DirectX::SimpleMath::Vector3* vertices = {};			// �� ���ؽ�
		int vertexSize;											// �� ���ؽ� ������
		unsigned char convexPolygonLimit = 4;					// ������ �޽ÿ� ������ ������ �ִ� �� ( �ּ� : 4�� �̻�, �ִ� 256�� �̸� )
	};
	struct MaterialResourceInfo
	{
		float staticFriction = 1.f;									// ���� ���� ��� ( 0.f ~ 1.f )
		float dynamicFriction = 1.f;								// ���� ���� ��� ( 0.f ~ 1.f )
		float restitution = 1.f;									// ���� ��� ( 0.f ~ 1.f ) 
		float density = 1.f;										// �е� ( 0.f ~ 1.f )
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
		unsigned int convexMeshHash = 0;						// ������ �޽� �ؽ� ��
	};
#pragma endregion

#pragma region CharacterController
	/// <summary>
	/// ĳ���� �����Ʈ : ĳ���� �̵��� ���õ� ������
	/// </summary>
	struct CharacterMovementInfo
	{
		float maxSpeed = 0.025f;								// �̵� �ִ� �ӵ� : ĳ���Ͱ� ������ �� �ִ� �ִ� �ӵ�
		float acceleration = 1.f;								// ���ӵ� : ĳ���Ͱ� �Է� ���� ���� �� �̵� ���ӵ�
		float staticFriction = 0.4f;							// ���� ���� ��� : ĳ���Ͱ� �̵� �� ������ �� ĳ���Ͱ� �޴� ������ ( 0.0f ~ 1.f )
		float dynamicFriction = 0.1f;							// ���� ���� ��� : �̵� �߿� ĳ���Ͱ� �޴� ������ ( 0.0f ~ 1.f )
		float jumpSpeed = 0.05f;								// ����(y��) �ӵ�
		float jumpXZAcceleration = 10.f;						// ���� �߿� �̵�(XZ��) ���ӵ� ��
		float jumpXZDeceleration = 0.1f;						// ���� �߿� �̵�(XZ��) ���� �� ( 0.0 ~ 1.0 )
		float gravityWeight = 0.2f;								// �⺻ �߷� ���� �� �� ������ ����ġ�� �� �ְ� ���� �� ���� �ٸ��� ������ �� �ֽ��ϴ�.
	};

	/// <summary>
	/// ĳ���� ��Ʈ�ѷ� : ĳ���� ��Ʈ�ѷ� ������ ���� ��Ʈ�ѷ� ������
	/// </summary>
	struct CharacterControllerInfo
	{
		unsigned int id = unregisterID;								// ĳ���� ��Ʈ�ѷ� ���̵�
		unsigned int layerNumber = 0;								// �浹 ��Ʈ���� ���̾� �ѹ�

		DirectX::SimpleMath::Vector3 position{ 0.f, 0.f, 0.f };		// ĳ���� ��Ʈ�ѷ��� ��ġ�ϴ� ó�� ���� ��ġ
		float height = 0.1f;										// ĳ���� ��Ʈ�ѷ�(ĸ��)�� ����
		float radius = 0.05f;										// ĳ���� ��Ʈ�ѷ�(ĸ��)�� ������
		float stepOffset = 0.0f;									// ĳ���� ��Ʈ�ѷ��� ������ �� �ִ� 
		float slopeLimit = 0.3f;									// ĳ���Ͱ� �ɾ� �ö� �� �ִ� �ִ� ����
		float contactOffset = 0.001f;								// ��Ʈ�ѷ��� ���� ������ : ��ġ ���е� ������ �����ϱ� ���� ����մϴ�.
	};
#pragma endregion

#pragma region CharacterPhysics
	struct JointAxisInfo
	{
		EArticulationMotion motion = EArticulationMotion::LIMITED;		// ��� ����
		float limitsLow = -60.f;										// ���� ( Limit�� �� ȸ�� �Ʒ� ���� ���� : -180.0 ~ 0.0 ) 
		float limitsHigh = 60.f;										// ���� ( Limit�� �� ȸ�� �� ���� ���� : 0.0 ~ 180.0 )
	};

	struct CharacterJointInfo
	{
		JointAxisInfo Swing1AxisInfo;					// Swing1( X���� �߽����� �� ȸ�� )
		JointAxisInfo Swing2AxisInfo;					// Swing2( Y���� �߽����� �� ȸ�� )
		JointAxisInfo TwistAxisInfo;					// Twist( Z���� �߽����� �� ȸ�� )
		DirectX::SimpleMath::Matrix localTransform;		// ����Ʈ�� ���� ��ǥ
		float stiffness = 1.f;							// ���� : ������ ��ǥ ��ġ�� �̵��Ϸ��� ���� ũ�� ( 0.f ~ 1.f )
		float damping = 1.f;							// ���� ��� : ��� ���� ���׷� ( ������ �����ϰ� �ε巴�� �����̵� �� �� �ְ� ) ( 0.f ~ 1.f )
		float maxForce = 1.f;							// �ִ� �� : ���� ����̺갡 ������ �� �ִ� �ִ� �� 
	};

	struct CharacterLinkInfo
	{
		std::string boneName;							// �ش� ��(��ũ)�� �̸�
		std::string parentBoneName;						// �θ� ��(��ũ)�� �̸�
		float density = 1.f;							// �е� ( 0.f ~ 1.f )
		CharacterJointInfo JointInfo;					// ����Ʈ ����
		DirectX::SimpleMath::Matrix localTransform;		// ���� ��ǥ
	};

	struct CharacterPhysicsInfo
	{
		std::string modelPath;							// �� ����(�� �����͸� ������ �ִ�) ���
		unsigned int id = unregisterID;					// ���̵�
		unsigned int layerNumber = 0;					// ���̾� �ѹ�
		DirectX::SimpleMath::Matrix worldTransform;		// ���� ��ǥ
		float staticFriction = 1.f;						// ���� ���� ��� ( 0.f ~ 1.f )
		float dynamicFriction = 1.f;					// ���� ���� ��� ( 0.f ~ 1.f )
		float restitution = 1.f;						// ���� ��� ( 0.f ~ 1.f )
		float density = 1.f;							// �е� ( 0.f ~ 1.f )
	};
#pragma endregion
}