#pragma once
#include "FQCommon.h"

#include <string>

namespace fq::physics
{
#pragma region EnumClass
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
	/// 관절 축
	/// </summary>
	enum class EArticulationAxis
	{
		SWING_1,
		SWING_2,
		TWIST
	};

	/// <summary>
	/// 관절 모션
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
	/// 물리 엔진 세팅 데이터 : 중력, 충돌 매트릭스 
	/// </summary>
	struct PhysicsEngineInfo
	{
		DirectX::SimpleMath::Vector3 gravity = {};
		int collisionMatrix[16] = {};
	};

	/// <summary>
	/// 콜리전 : 충돌 데이터
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
	/// 물리 엔진에서 리지드 바디 정보들을 주고 받는 GetSet 구조체
	/// </summary>
	struct RigidBodyGetSetData
	{
		DirectX::SimpleMath::Matrix transform = {};				// 트랜스폼 정보입니다.
		DirectX::SimpleMath::Vector3 linearVelocity = {};		// 값이 0이면 StaticBody 입니다.
		DirectX::SimpleMath::Vector3 angularVelocity = {};		// 값이 0이면 StaticBody 입니다.
		unsigned int myLayerNumber = UINT_MAX;
	};

	struct CharacterControllerGetSetData
	{
		DirectX::SimpleMath::Vector3 position = {};				// 캐릭터 컨트롤러의 위치
		DirectX::SimpleMath::Vector3 scale = {1.f,1.f,1.f};		
		DirectX::SimpleMath::Quaternion rotation = {};
		unsigned int myLayerNumber = UINT_MAX;
	};

	struct CharacterMovementGetSetData
	{
		DirectX::SimpleMath::Vector3 velocity = {};				// 캐릭터 컨트롤러의 x,y,z축 속도
		bool isFall = false;									// 캐릭터가 떨어지고 있는지 체크 변수
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
		DirectX::SimpleMath::Vector3* vertices = nullptr;	// 모델 버텍스
		int vertexSize = 0;									// 모델 버텍스 사이즈
		unsigned char convexPolygonLimit = 4;				// 컨벡스 메시에 생성할 폴리곤 최대 수 ( 최소 : 4개 이상, 최대 256개 미만 )
	};
	struct MaterialResourceInfo
	{
		float staticFriction = 1.f;							// 정적 마찰 계수 ( 0.f ~ 1.f )
		float dynamicFriction = 1.f;						// 동적 마찰 계수 ( 0.f ~ 1.f )
		float restitution = 1.f;							// 복원 계수 ( 0.f ~ 1.f ) 
		float density = 1.f;								// 밀도 ( 0.f ~ 1.f )
	};
#pragma endregion

#pragma region Collider
	constexpr unsigned int unregisterID = 0;

	/// <summary>
	/// 콜라이더 : 물리 충돌 처리를 위한 오브젝트 형태
	/// </summary>
	struct ColliderInfo
	{
		unsigned int id = unregisterID;
		unsigned int layerNumber = 0;
		fq::common::Transform	collisionTransform = {};
		float staticFriction = 1.f;							// 정적 마찰 계수
		float dynamicFriction = 1.f;						// 동적 마찰 계수
		float restitution = 1.f;							// 복원 계수
		float density = 1.f;								// 밀도
	};

	struct BoxColliderInfo
	{
		ColliderInfo colliderInfo;
		DirectX::SimpleMath::Vector3 boxExtent = {};		// 길이
	};

	struct SphereColliderInfo
	{
		ColliderInfo colliderInfo;
		float raidus = 1.f;									// 반지름
	};

	struct CapsuleColliderInfo
	{
		ColliderInfo colliderInfo;
		float raidus = 1.f;									// 반지름
		float halfHeight = 1.f;								// 높이
	};

	struct ConvexMeshColliderInfo
	{
		ColliderInfo colliderInfo;
		DirectX::SimpleMath::Vector3* vertices = nullptr;	// 모델 버텍스
		int vertexSize = 0;									// 모델 버텍스 사이즈
		unsigned char convexPolygonLimit = 4;				// 컨벡스 메시에 생성할 폴리곤 최대 수 ( 최소 : 4개 이상, 최대 256개 미만 )
		unsigned int convexMeshHash = 0;					// 컨벡스 메시 해쉬 값
	};

	struct TriangleMeshColliderInfo
	{
		ColliderInfo colliderInfo;
		unsigned int triangleMeshHash = 0;					// 트라이앵글 메시 해쉬 값
		DirectX::SimpleMath::Vector3* vertices = nullptr;	// 모델 버텍스
		int vertexSize = 0;									// 모델 버텍스 사이즈
		unsigned int* indices = nullptr;					// 모델 인덱스
		int indexSize = 0;									// 모델 인덱스 사이즈
	};

	struct HeightFieldColliderInfo
	{
		ColliderInfo colliderInfo;
		unsigned int heightFieldMeshHash = 0;				// 높이맵 해쉬 값
		int* height = nullptr;								// 높이 맵
		unsigned int numCols = 0;							// 가로 (열)
		unsigned int numRows = 0;							// 세로 (행)
		float heightScale = 1.f;							// 높이 스케일
		float rowScale = 1.f;								// 가로(열) 스케일
		float colScale = 1.f;								// 세로(행) 스케일
	};
#pragma endregion

#pragma region CharacterController
	/// <summary>
	/// 캐릭터 무브먼트 : 캐릭터 이동에 관련된 데이터
	/// </summary>
	struct CharacterMovementInfo
	{
		float maxSpeed = 0.025f;							// 이동 최대 속도 : 캐릭터가 움직일 수 있는 최대 속도
		float acceleration = 1.f;							// 가속도 : 캐릭터가 입력 값을 받을 때 이동 가속도
		float staticFriction = 0.4f;						// 정적 마찰 계수 : 캐릭터가 이동 중 멈췄을 때 캐릭터가 받는 마찰력 ( 0.0f ~ 1.f )
		float dynamicFriction = 0.1f;						// 동적 마찰 계수 : 이동 중에 캐릭터가 받는 마찰력 ( 0.0f ~ 1.f )
		float jumpSpeed = 0.05f;							// 점프(y축) 속도
		float jumpXZAcceleration = 10.f;					// 점프 중에 이동(XZ축) 가속도 값
		float jumpXZDeceleration = 0.1f;					// 점프 중에 이동(XZ축) 감속 값 ( 0.0 ~ 1.0 )
		float gravityWeight = 0.2f;							// 기본 중력 값을 줄 수 있지만 가중치를 더 주고 싶을 때 값을 다르게 세팅할 수 있습니다.
	};

	/// <summary>
	/// 캐릭터 컨트롤러 : 캐릭터 컨트롤러 생성을 위한 컨트롤러 데이터
	/// </summary>
	struct CharacterControllerInfo
	{
		unsigned int id = unregisterID;								// 캐릭터 컨트롤러 아이디
		unsigned int layerNumber = 0;								// 충돌 매트릭스 레이어 넘버

		DirectX::SimpleMath::Vector3 position{ 0.f, 0.f, 0.f };		// 캐릭터 컨트롤러가 위치하는 처음 생성 위치
		float height = 0.1f;										// 캐릭터 컨트롤러(캡슐)의 높이
		float radius = 0.05f;										// 캐릭터 컨트롤러(캡슐)의 반지름
		float stepOffset = 0.0f;									// 캐릭터 컨트롤러가 지나갈 수 있는 
		float slopeLimit = 0.3f;									// 캐릭터가 걸어 올라갈 수 있는 최대 기울기
		float contactOffset = 0.001f;								// 컨트롤러의 접촉 오프셋 : 수치 정밀도 문제를 방지하기 위해 사용합니다.
	};
#pragma endregion

#pragma region CharacterPhysics
	struct JointAxisInfo
	{
		EArticulationMotion motion = EArticulationMotion::LIMITED;		// 모션 제한
		float limitsLow = -60.f;										// 범위 ( Limit일 때 회전 아랫 각도 범위 : -180.0 ~ 0.0 ) 
		float limitsHigh = 60.f;										// 범위 ( Limit일 때 회전 윗 각도 범위 : 0.0 ~ 180.0 )
	};

	struct JointInfo
	{
		JointAxisInfo Swing1AxisInfo;						// Swing1( X축을 중심으로 한 회전 )
		JointAxisInfo Swing2AxisInfo;						// Swing2( Y축을 중심으로 한 회전 )
		JointAxisInfo TwistAxisInfo;						// Twist( Z축을 중심으로 한 회전 )
		DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;	// 조인트의 로절 좌표
		float stiffness = 1.f;								// 강성 : 관절이 목표 위치로 이동하려는 힘의 크기 ( 0.f ~ 1.f )
		float damping = 1.f;								// 감쇠 계수 : 운동에 대한 저항력 ( 진동을 방지하고 부드럽게 움직이동 할 수 있게 ) ( 0.f ~ 1.f )
		float maxForce = 1.f;								// 최대 힘 : 관절 드라이브가 적용할 수 있는 최대 힘 
	};

	struct LinkInfo
	{
		std::string boneName = {};							// 해당 본(링크)의 이름
		std::string parentBoneName = {};					// 부모 본(링크)의 이름
		float density = 1.f;								// 밀도 ( 0.f ~ 1.f )
		DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;	// 로컬 좌표
		JointInfo jointInfo;								// 조인트 정보
	};

	struct ArticulationInfo
	{
		unsigned int id = unregisterID;						// 아이디
		unsigned int layerNumber = 0;						// 충돌 레이어 넘버
		DirectX::SimpleMath::Matrix worldTransform = DirectX::SimpleMath::Matrix::Identity;	// 월드 좌표
		float staticFriction = 1.f;							// 정적 마찰 계수 ( 0.f ~ 1.f )
		float dynamicFriction = 1.f;						// 동적 마찰 계수 ( 0.f ~ 1.f )
		float restitution = 1.f;							// 복원 계수 ( 0.f ~ 1.f )
		float density = 1.f;								// 밀도 ( 0.f ~ 1.f )
	};
#pragma endregion

#pragma region PhysicsCloth
	struct PhysicsClothMaterialInfo
	{
		float friction = 0.2f;							// 마찰 계수 : 천의 표면과 다른 물체 간의 마찰 정도를 결정합니다.
		float damping = 0.01f;							// 감쇠 계수 : 천의 운동 에너지를 감쇠시키는 정도를 나타냅니다.
		float adhesion = 1e+3f;							// 점착력	: 천이 다른 표면에 붙는 정도를 나타냅니다.
		float viscosity = 0.005f;						// 점성		: 천의 내부 저항으로 인해 움직임이 저하되는 정도를 나타냅니다.
		float vorticityConfinement = 0.5f;				// 와도 구속 : 천의 소용돌이 효과를 제어하는 데 사용됩니다.
		float surfaceTension = 0.01f;					// 표면 장력 : 천의 표면에서 발생하는 장력을 나타냅니다.
		float cohesion = 1.f;							// 응집력	: 천의 입자들이 서로 모이는 힘을 나타냅니다.
		float lift = 0.01f;								// 양력		: 천이 위로 떠오르는 힘을 나타냅니다.
		float drag = 0.01f;								// 항력		: 천이 움직일 때 저항을 받는 힘을 나타냅니다.
		float cflCoefficient = 1.f;						// CFL 계수	: 천 시뮬레이션에서 안정성을 제어하는 계수입니다.
		float gravityScale = 1.f;						// 중력 스케일 : 천에 작용하는 중력의 크기를 조정합니다.
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