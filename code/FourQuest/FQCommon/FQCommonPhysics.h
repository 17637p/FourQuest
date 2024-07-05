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
		FREE = 2,

		END
	};
#pragma endregion

	/// <summary>
	/// ���� ���� ���� ������ : �߷�, �浹 ��Ʈ���� 
	/// </summary>
	struct PhysicsEngineInfo
	{
		DirectX::SimpleMath::Vector3 gravity = {};
		int collisionMatrix[16] = {};
	};

	/// <summary>
	/// �ݸ��� : �浹 ������
	/// </summary>
	struct CollisionData
	{
		unsigned int myId = 0;
		unsigned int otherId = 0;
		unsigned int myLayerNumber = 0;
		unsigned int otherLayerNumber = 0;
		std::vector<DirectX::SimpleMath::Vector3> ContectPoints = {};
		bool isDead = false;
	};

	struct RayCastData
	{
		unsigned int hitSize = 0;
		unsigned int myId = 0;
		unsigned int myLayerNumber = 0;
		std::vector<unsigned int> id = {};
		std::vector<unsigned int> layerNumber = {};
		std::vector<DirectX::SimpleMath::Vector3> contectPoints = {};
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
		unsigned int myLayerNumber = UINT_MAX;
	};

	struct CharacterControllerGetSetData
	{
		DirectX::SimpleMath::Vector3 position = {};				// ĳ���� ��Ʈ�ѷ��� ��ġ
		DirectX::SimpleMath::Vector3 scale = {1.f,1.f,1.f};		
		DirectX::SimpleMath::Quaternion rotation = {};
		unsigned int myLayerNumber = UINT_MAX;
	};

	struct CharacterMovementGetSetData
	{
		DirectX::SimpleMath::Vector3 velocity = {};				// ĳ���� ��Ʈ�ѷ��� x,y,z�� �ӵ�
		bool isFall = false;									// ĳ���Ͱ� �������� �ִ��� üũ ����
	};

	struct PhysicsClothGetData
	{
		DirectX::SimpleMath::Matrix worldTransform = {};
		unsigned int myLayerNumber = UINT_MAX;
	};
	struct PhysicsClothSetData
	{
		DirectX::SimpleMath::Matrix worldTransform = {};
		unsigned int myLayerNumber = UINT_MAX;
	};
#pragma endregion

#pragma region Resource
	struct ConvexMeshResoureceInfo
	{
		DirectX::SimpleMath::Vector3* vertices = nullptr;	// �� ���ؽ�
		int vertexSize = 0;									// �� ���ؽ� ������
		unsigned char convexPolygonLimit = 4;				// ������ �޽ÿ� ������ ������ �ִ� �� ( �ּ� : 4�� �̻�, �ִ� 256�� �̸� )
	};
	struct MaterialResourceInfo
	{
		float staticFriction = 1.f;							// ���� ���� ��� ( 0.f ~ 1.f )
		float dynamicFriction = 1.f;						// ���� ���� ��� ( 0.f ~ 1.f )
		float restitution = 1.f;							// ���� ��� ( 0.f ~ 1.f ) 
		float density = 1.f;								// �е� ( 0.f ~ 1.f )
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
		fq::common::Transform	collisionTransform = {};
		float staticFriction = 1.f;							// ���� ���� ���
		float dynamicFriction = 1.f;						// ���� ���� ���
		float restitution = 1.f;							// ���� ���
		float density = 1.f;								// �е�
	};

	struct BoxColliderInfo
	{
		ColliderInfo colliderInfo;
		DirectX::SimpleMath::Vector3 boxExtent = {};		// ����
	};

	struct SphereColliderInfo
	{
		ColliderInfo colliderInfo;
		float raidus = 1.f;									// ������
	};

	struct CapsuleColliderInfo
	{
		ColliderInfo colliderInfo;
		float raidus = 1.f;									// ������
		float halfHeight = 1.f;								// ����
	};

	struct ConvexMeshColliderInfo
	{
		ColliderInfo colliderInfo;
		DirectX::SimpleMath::Vector3* vertices = nullptr;	// �� ���ؽ�
		int vertexSize = 0;									// �� ���ؽ� ������
		unsigned char convexPolygonLimit = 4;				// ������ �޽ÿ� ������ ������ �ִ� �� ( �ּ� : 4�� �̻�, �ִ� 256�� �̸� )
		unsigned int convexMeshHash = 0;					// ������ �޽� �ؽ� ��
	};

	struct TriangleMeshColliderInfo
	{
		ColliderInfo colliderInfo;
		unsigned int triangleMeshHash = 0;					// Ʈ���̾ޱ� �޽� �ؽ� ��
		DirectX::SimpleMath::Vector3* vertices = nullptr;	// �� ���ؽ�
		int vertexSize = 0;									// �� ���ؽ� ������
		unsigned int* indices = nullptr;					// �� �ε���
		int indexSize = 0;									// �� �ε��� ������
	};

	struct HeightFieldColliderInfo
	{
		ColliderInfo colliderInfo;
		unsigned int heightFieldMeshHash = 0;				// ���̸� �ؽ� ��
		int* height = nullptr;								// ���� ��
		unsigned int numCols = 0;							// ���� (��)
		unsigned int numRows = 0;							// ���� (��)
		float heightScale = 1.f;							// ���� ������
		float rowScale = 1.f;								// ����(��) ������
		float colScale = 1.f;								// ����(��) ������
	};
#pragma endregion

#pragma region CharacterController
	/// <summary>
	/// ĳ���� �����Ʈ : ĳ���� �̵��� ���õ� ������
	/// </summary>
	struct CharacterMovementInfo
	{
		float maxSpeed = 0.025f;							// �̵� �ִ� �ӵ� : ĳ���Ͱ� ������ �� �ִ� �ִ� �ӵ�
		float acceleration = 1.f;							// ���ӵ� : ĳ���Ͱ� �Է� ���� ���� �� �̵� ���ӵ�
		float staticFriction = 0.4f;						// ���� ���� ��� : ĳ���Ͱ� �̵� �� ������ �� ĳ���Ͱ� �޴� ������ ( 0.0f ~ 1.f )
		float dynamicFriction = 0.1f;						// ���� ���� ��� : �̵� �߿� ĳ���Ͱ� �޴� ������ ( 0.0f ~ 1.f )
		float jumpSpeed = 0.05f;							// ����(y��) �ӵ�
		float jumpXZAcceleration = 10.f;					// ���� �߿� �̵�(XZ��) ���ӵ� ��
		float jumpXZDeceleration = 0.1f;					// ���� �߿� �̵�(XZ��) ���� �� ( 0.0 ~ 1.0 )
		float gravityWeight = 0.2f;							// �⺻ �߷� ���� �� �� ������ ����ġ�� �� �ְ� ���� �� ���� �ٸ��� ������ �� �ֽ��ϴ�.
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

	struct JointInfo
	{
		JointAxisInfo Swing1AxisInfo;						// Swing1( X���� �߽����� �� ȸ�� )
		JointAxisInfo Swing2AxisInfo;						// Swing2( Y���� �߽����� �� ȸ�� )
		JointAxisInfo TwistAxisInfo;						// Twist( Z���� �߽����� �� ȸ�� )
		DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;	// ����Ʈ�� ���� ��ǥ
		float stiffness = 1.f;								// ���� : ������ ��ǥ ��ġ�� �̵��Ϸ��� ���� ũ�� ( 0.f ~ 1.f )
		float damping = 1.f;								// ���� ��� : ��� ���� ���׷� ( ������ �����ϰ� �ε巴�� �����̵� �� �� �ְ� ) ( 0.f ~ 1.f )
		float maxForce = 1.f;								// �ִ� �� : ���� ����̺갡 ������ �� �ִ� �ִ� �� 
	};

	struct LinkInfo
	{
		std::string boneName = {};							// �ش� ��(��ũ)�� �̸�
		std::string parentBoneName = {};					// �θ� ��(��ũ)�� �̸�
		float density = 1.f;								// �е� ( 0.f ~ 1.f )
		DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;	// ���� ��ǥ
		JointInfo jointInfo;								// ����Ʈ ����
	};

	struct ArticulationInfo
	{
		unsigned int id = unregisterID;						// ���̵�
		unsigned int layerNumber = 0;						// �浹 ���̾� �ѹ�
		DirectX::SimpleMath::Matrix worldTransform = DirectX::SimpleMath::Matrix::Identity;	// ���� ��ǥ
		float staticFriction = 1.f;							// ���� ���� ��� ( 0.f ~ 1.f )
		float dynamicFriction = 1.f;						// ���� ���� ��� ( 0.f ~ 1.f )
		float restitution = 1.f;							// ���� ��� ( 0.f ~ 1.f )
		float density = 1.f;								// �е� ( 0.f ~ 1.f )
	};
#pragma endregion

#pragma region PhysicsCloth
	struct PhysicsClothMaterialInfo
	{
		float friction = 0.2f;							// ���� ��� : õ�� ǥ��� �ٸ� ��ü ���� ���� ������ �����մϴ�.
		float damping = 0.01f;							// ���� ��� : õ�� � �������� �����Ű�� ������ ��Ÿ���ϴ�.
		float adhesion = 1e+3f;							// ������	: õ�� �ٸ� ǥ�鿡 �ٴ� ������ ��Ÿ���ϴ�.
		float viscosity = 0.005f;						// ����		: õ�� ���� �������� ���� �������� ���ϵǴ� ������ ��Ÿ���ϴ�.
		float vorticityConfinement = 0.5f;				// �͵� ���� : õ�� �ҿ뵹�� ȿ���� �����ϴ� �� ���˴ϴ�.
		float surfaceTension = 0.01f;					// ǥ�� ��� : õ�� ǥ�鿡�� �߻��ϴ� ����� ��Ÿ���ϴ�.
		float cohesion = 1.f;							// ������	: õ�� ���ڵ��� ���� ���̴� ���� ��Ÿ���ϴ�.
		float lift = 0.01f;								// ���		: õ�� ���� �������� ���� ��Ÿ���ϴ�.
		float drag = 0.01f;								// �׷�		: õ�� ������ �� ������ �޴� ���� ��Ÿ���ϴ�.
		float cflCoefficient = 1.f;						// CFL ���	: õ �ùķ��̼ǿ��� �������� �����ϴ� ����Դϴ�.
		float gravityScale = 1.f;						// �߷� ������ : õ�� �ۿ��ϴ� �߷��� ũ�⸦ �����մϴ�.
	};

	struct PhysicsClothInfo
	{
		unsigned int id = unregisterID;
		unsigned int layerNumber = 0;

		PhysicsClothMaterialInfo materialInfo;

		DirectX::SimpleMath::Matrix worldTransform = {};
		float clothMass = 1.f;
		float restOffset = 1.f;

		void* vertexBuffer = nullptr;							// D3D11Buffer*
		void* indexBuffer = nullptr;							// D3D11Buffer*
	};
#pragma endregion
}