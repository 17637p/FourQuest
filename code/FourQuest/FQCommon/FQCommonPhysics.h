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
		FREE = 2
	};
#pragma endregion

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
		std::vector<DirectX::SimpleMath::Vector3> ContectPoints;
		bool isDead = false;
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
	};

	struct CharacterControllerGetSetData
	{
		DirectX::SimpleMath::Vector3 position;					// 캐릭터 컨트롤러의 위치
	};

	struct CharacterMovementGetSetData
	{
		DirectX::SimpleMath::Vector3 velocity;					// 캐릭터 컨트롤러의 x,y,z축 속도
		bool isFall;											// 캐릭터가 떨어지고 있는지 체크 변수
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
		fq::common::Transform collisionTransform;
		float staticFriction = 1.f;									// 정적 마찰 계수
		float dynamicFriction = 1.f;								// 동적 마찰 계수
		float restitution = 1.f;									// 복원 계수
		float density = 1.f;										// 밀도
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
		DirectX::SimpleMath::Vector3* vertices;					// 모델 버텍스
		int vertexSize;											// 모델 버텍스 사이즈
		unsigned char convexPolygonLimit = 4;					// 컨벡스 메시에 생성할 폴리곤 최대 수 ( 최소 : 4개 이상, 최대 256개 미만 )
	};
#pragma endregion

#pragma region CharacterController
	/// <summary>
	/// 캐릭터 무브먼트 : 캐릭터 이동에 관련된 데이터
	/// </summary>
	struct CharacterMovementInfo
	{
		float maxSpeed = 5.f;									// 이동 최대 속도 : 캐릭터가 움직일 수 있는 최대 속도
		float acceleration = 100.f;								// 가속도 : 캐릭터가 입력 값을 받을 때 이동 가속도
		float staticFriction = 0.4f;							// 정적 마찰 계수 : 캐릭터가 이동 중 멈췄을 때 캐릭터가 받는 마찰력 ( 0.0f ~ 1.f )
		float dynamicFriction = 0.1f;							// 동적 마찰 계수 : 이동 중에 캐릭터가 받는 마찰력 ( 0.0f ~ 1.f )
		float jumpSpeed = 5.f;									// 점프(y축) 속도
		float jumpXZAcceleration = 10.f;						// 점프 중에 이동(XZ축) 가속도 값
		float jumpXZDeceleration = 0.1f;						// 점프 중에 이동(XZ축) 감속 값 ( 0.0 ~ 1.0 )
		float gravityWeight = 9.8f;								// 기본 중력 값을 줄 수 있지만 가중치를 더 주고 싶을 때 값을 다르게 세팅할 수 있습니다.
	};

	/// <summary>
	/// 캐릭터 컨트롤러 : 캐릭터 컨트롤러 생성을 위한 컨트롤러 데이터
	/// </summary>
	struct CharacterControllerInfo
	{
		unsigned int id = unregisterID;							// 캐릭터 컨트롤러 아이디
		unsigned int layerNumber = 0;							// 충돌 매트릭스 레이어 넘버

		DirectX::SimpleMath::Vector3 position{ 0.f, 0.f, 0.f };	// 캐릭터 컨트롤러가 위치하는 처음 생성 위치
		float height = 30.f;									// 캐릭터 컨트롤러(캡슐)의 높이
		float radius = 20.f;									// 캐릭터 컨트롤러(캡슐)의 반지름
		float stepOffset = 0.0f;								// 캐릭터 컨트롤러가 지나갈 수 있는 
		float slopeLimit = 0.7f;								// 캐릭터가 걸어 올라갈 수 있는 최대 기울기
		float contactOffset = 0.1f;								// 컨트롤러의 접촉 오프셋 : 수치 정밀도 문제를 방지하기 위해 사용합니다.
	};
#pragma endregion

#pragma region CharacterPhysics
	struct JointAxisInfo
	{
		EArticulationMotion motion;						// 모션 제한
		float limitsLow;								// 범위 ( Limit일 때 회전 아랫 범위 : 0.0 ~ 1.0 ) 
		float limitsHigh;								// 범위 ( Limit일 때 회전 윗 범위 : 0.0 ~ 1.0 )

	};

	struct CharacterJointInfo
	{
		JointAxisInfo jointSwing1AxisInfo;				// Swing1( X축을 중심으로 한 회전 )
		JointAxisInfo jointSwing2AxisInfo;				// Swing2( Y축을 중심으로 한 회전 )
		JointAxisInfo jointTwistAxisInfo;				// Twist( Z축을 중심으로 한 회전 )
		DirectX::SimpleMath::Matrix localTransform;		// 조인트의 로절 좌표
		float stiffness = 100.f;						// 강성 : 관절이 목표 위치로 이동하려는 힘의 크기
		float damping = 10.f;							// 감쇠 계수 : 운동에 대한 저항력 ( 진동을 방지하고 부드럽게 움직이동 할 수 있게 )
		float maxForce = 1000.f;						// 최대 힘 : 관절 드라이브가 적용할 수 있는 최대 힘
	};

	struct CharacterLinkInfo
	{
		std::string boneName;							// 해당 본(링크)의 이름
		std::string parentBoneName;						// 부모 본(링크)의 이름
		float density;
		CharacterJointInfo characterJointInfo;			// 조인트 정보
		DirectX::SimpleMath::Matrix localTransform;		// 로컬 좌표
	};

	struct CharacterPhysicsInfo
	{
		std::string modelPath;
		unsigned int id = unregisterID;					// 아이디
		unsigned int layerNumber = 0;					// 레이어 넘버
		DirectX::SimpleMath::Matrix worldTransform;		// 월드 좌표
		float staticFriction = 1.f;						// 정적 마찰 계수
		float dynamicFriction = 1.f;					// 동적 마찰 계수
		float restitution = 1.f;						// 복원 계수
		float density = 1.f;							// 밀도
	};
#pragma endregion
}