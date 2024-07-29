#pragma once

#include <directxtk\SimpleMath.h>

#include "FQPhysics.h"

namespace fq::physics
{
	class CharacterMovement
	{
	public:
		CharacterMovement();
		~CharacterMovement();

		/// <summary>
		/// 캐릭터 무브먼트 초기화 하는 함수
		/// </summary>
		/// <param name="info"> 캐릭터 무브먼트 초기화 데이터 </param>
		void initialize(const CharacterMovementInfo& info);

		/// <summary>
		/// 입력 받은 키값을 바탕으로 변위 벡터 계산
		/// </summary>
		/// <param name="deltaTime"> 경과 시간 </param>
		/// <param name="input"> 입력 받은 키값 </param>
		void Update(float deltaTime, const DirectX::SimpleMath::Vector3& input, bool isDynamic);

		/// <summary>
		/// 점프하는 함수
		/// </summary>
		void Jump();

		/// <summary>
		/// 가속도, 최대속도 등을 계산하여 변위 벡터 계산
		/// </summary>
		/// <returns></returns>
		void Compute(float deltaTime);
		
		/// <summary>
		/// 변위 벡터를 PxVec3 자료형으로 변환
		/// </summary>
		void CopyDirectionToPxVec3(physx::PxVec3& direction);

		/// <summary>
		/// 현재 속도가 최대 속도를 넘는 순간 속도를 제한하는 함수.
		/// </summary>
		void LimitVelocity();


		inline const DirectX::SimpleMath::Vector3& GetVelocity();
		inline const DirectX::SimpleMath::Vector3& GetDisplacementVector();
		inline const bool& GetIsFall();
		inline const float& GetMaxSpeed();
		inline const float& GetSpeed();
		inline const float& GetAcceleration();
		inline const float& GetStaticFriction();
		inline const float& GetDynamicFriction();
		inline const float& GetJumpSpeed();
		inline const float& GetJumpXZAcceleration();
		inline const float& GetJumpXZDeceleration();
		inline const float& GetGravityWeight();
		inline void SetVelocity(const DirectX::SimpleMath::Vector3& velocity);
		inline void SetIsFall(const bool& isFall);
		inline void SetMaxSpeed(const float& maxSpeed);
		inline void SetAcceleration(const float& acceleration);
		inline void SetStaticFriction(const float& staticFriction);
		inline void SetDynamicFriction(const float& dynamicFriction);
		inline void SetJumpSpeed(const float& jumpSpeed);
		inline void SetJumpXZAcceleration(const float& jumpXZAcceleration);
		inline void SetJumpXZDeceleration(const float& jumpXZDeceleration);
		inline void SetGravityWeight(const float& gravityWeight);


	private:
		bool mIsFall;

		DirectX::SimpleMath::Vector3 mVelocity;
		DirectX::SimpleMath::Vector3 mDisplacementVector;

		float mMaxSpeed;
		float mSpeed;
		float mAcceleration;
		float mStaticFriction;
		float mDynamicFriction;
		float mJumpSpeed;
		float mJumpXZAcceleration;
		float mJumpXZDeceleration;
		float mGravityWeight;
		float mMinDistance;
	};


#pragma region GetSet

	const DirectX::SimpleMath::Vector3& CharacterMovement::GetVelocity()
	{
		return mVelocity;
	}
	const DirectX::SimpleMath::Vector3& CharacterMovement::GetDisplacementVector()
	{
		return mDisplacementVector;
	}
	const bool& CharacterMovement::GetIsFall()
	{
		return mIsFall;
	}
	const float& CharacterMovement::GetMaxSpeed()
	{
		return mMaxSpeed;
	}
	const float& CharacterMovement::GetSpeed()
	{
		return mSpeed;
	}
	const float& CharacterMovement::GetAcceleration()
	{
		return mAcceleration;
	}
	const float& CharacterMovement::GetStaticFriction()
	{
		return mStaticFriction;
	}
	const float& CharacterMovement::GetDynamicFriction()
	{
		return mDynamicFriction;
	}
	const float& CharacterMovement::GetJumpSpeed()
	{
		return mJumpSpeed;
	}
	const float& CharacterMovement::GetJumpXZAcceleration()
	{
		return mJumpXZAcceleration;
	}
	const float& CharacterMovement::GetJumpXZDeceleration()
	{
		return mJumpXZDeceleration;
	}
	const float& CharacterMovement::GetGravityWeight()
	{
		return mGravityWeight;
	}

	void CharacterMovement::SetVelocity(const DirectX::SimpleMath::Vector3& velocity)
	{
		mVelocity = velocity;
	}
	void CharacterMovement::SetIsFall(const bool& isFall)
	{
		mIsFall = isFall;
	}
	void CharacterMovement::SetMaxSpeed(const float& maxSpeed)
	{
		mMaxSpeed = maxSpeed;
	}
	void CharacterMovement::SetAcceleration(const float& acceleration)
	{
		mAcceleration = acceleration;
	}
	void CharacterMovement::SetStaticFriction(const float& staticFriction)
	{
		mStaticFriction = staticFriction;
	}
	void CharacterMovement::SetDynamicFriction(const float& dynamicFriction)
	{
		mDynamicFriction = dynamicFriction;
	}
	void CharacterMovement::SetJumpSpeed(const float& jumpSpeed)
	{
		mJumpSpeed = jumpSpeed;
	}
	void CharacterMovement::SetJumpXZAcceleration(const float& jumpXZAcceleration)
	{
		mJumpXZAcceleration = jumpXZAcceleration;
	}
	void CharacterMovement::SetJumpXZDeceleration(const float& jumpXZDeceleration)
	{
		mJumpXZDeceleration = jumpXZDeceleration;
	}
	void CharacterMovement::SetGravityWeight(const float& gravityWeight)
	{
		mGravityWeight = gravityWeight;
	}

#pragma endregion

}