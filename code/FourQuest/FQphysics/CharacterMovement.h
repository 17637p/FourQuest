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
		/// ĳ���� �����Ʈ �ʱ�ȭ �ϴ� �Լ�
		/// </summary>
		/// <param name="info"> ĳ���� �����Ʈ �ʱ�ȭ ������ </param>
		void initialize(const CharacterMovementInfo& info);

		/// <summary>
		/// �Է� ���� Ű���� �������� ���� ���� ���
		/// </summary>
		/// <param name="deltaTime"> ��� �ð� </param>
		/// <param name="input"> �Է� ���� Ű�� </param>
		void Update(float deltaTime, const DirectX::SimpleMath::Vector3& input, bool isDynamic);

		/// <summary>
		/// �����ϴ� �Լ�
		/// </summary>
		void Jump();

		/// <summary>
		/// ���ӵ�, �ִ�ӵ� ���� ����Ͽ� ���� ���� ���
		/// </summary>
		/// <returns></returns>
		void Compute(float deltaTime);
		
		/// <summary>
		/// ���� ���͸� PxVec3 �ڷ������� ��ȯ
		/// </summary>
		void CopyDirectionToPxVec3(physx::PxVec3& direction);

		/// <summary>
		/// ���� �ӵ��� �ִ� �ӵ��� �Ѵ� ���� �ӵ��� �����ϴ� �Լ�.
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