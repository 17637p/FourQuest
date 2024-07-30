#include "CharacterMovement.h"

#include <iostream>
#include <spdlog/spdlog.h>

float square(float x)
{
	if (x < 0)
		return (x = -1 * (x * x));
	else
		return (x * x);
}

namespace fq::physics
{
	CharacterMovement::CharacterMovement()
		: mMaxSpeed()
		, mSpeed()
		, mAcceleration()
		, mStaticFriction()
		, mDynamicFriction()
		, mJumpSpeed()
		, mJumpXZAcceleration()
		, mJumpXZDeceleration()
		, mGravityWeight()
		, mMinDistance()
		, mIsFall()
		, mVelocity()
		, mDisplacementVector()
	{

	}

	CharacterMovement::~CharacterMovement()
	{

	}

	void CharacterMovement::initialize(const CharacterMovementInfo& info)
	{
		mMaxSpeed = info.maxSpeed;
		mAcceleration = info.acceleration;
		mStaticFriction = info.staticFriction;
		mDynamicFriction = info.dynamicFriction;
		mJumpSpeed = info.jumpSpeed;
		mJumpXZAcceleration = info.jumpXZAcceleration;
		mJumpXZDeceleration = info.jumpXZDeceleration;
		mGravityWeight = info.gravityWeight;
		mMinDistance = 0.1f;
	}

	void CharacterMovement::Update(float deltaTime, const DirectX::SimpleMath::Vector3& input, bool isDynamic)
	{
		if (!mIsFall)
		{
			// ���� ������ ��� ���� �����¿� ���� �ӵ��� ���ҽ�Ų��.
			// ���� ������ ��� ���� �����¿� ���� �ӵ��� ���ҽ�Ų��.
			if (input.x == 0 && !isDynamic)
				mVelocity.x = std::lerp(mVelocity.x, 0.f, mStaticFriction);
			else
				mVelocity.x = std::lerp(mVelocity.x, 0.f, mDynamicFriction);
			
			if (input.z == 0 && !isDynamic)
				mVelocity.z = std::lerp(mVelocity.z, 0.f, mStaticFriction);
			else
				mVelocity.z = std::lerp(mVelocity.z, 0.f, mDynamicFriction);

			mVelocity.x += (input.x * mAcceleration * deltaTime);
			mVelocity.z += (input.z * mAcceleration * deltaTime);
		}
		else
		{
			// ���� ������ ��� ���� XZ ���ӿ� ���� �ӵ��� ���ҽ�Ų��.
			if (input.x == 0)
				mVelocity.x = std::lerp(mVelocity.x, 0.f, mJumpXZDeceleration);
			if (input.z == 0)
				mVelocity.z = std::lerp(mVelocity.z, 0.f, mJumpXZDeceleration);

			mVelocity.x += (input.x * mJumpXZAcceleration * deltaTime);
			mVelocity.z += (input.z * mJumpXZAcceleration * deltaTime);
		}

		// �ӵ� ���� ����
		LimitVelocity();

		if (input.y != 0 && !mIsFall)
			Jump();

		// �ӵ� ���
		Compute(deltaTime);
	}

	void CharacterMovement::Jump()
	{
		mVelocity.y = mJumpSpeed;
	}

	void CharacterMovement::LimitVelocity()
	{
		mVelocity.x = std::clamp(mVelocity.x, -mMaxSpeed, mMaxSpeed);
		mVelocity.z = std::clamp(mVelocity.z, -mMaxSpeed, mMaxSpeed);
	}

	void CharacterMovement::Compute(float deltaTime)
	{
		// �߷� ����
		if (mIsFall)
			mVelocity.y -= mGravityWeight * deltaTime;

		// ���� �ӵ� ���
		mSpeed = sqrt(mVelocity.x * mVelocity.x + mVelocity.z * mVelocity.z);

		// �ִ� �ӵ� ����
		if (mSpeed > mMaxSpeed)
			mSpeed = mMaxSpeed;

		// ���� ���� ���
		float triangleFunction = sqrt(mVelocity.x * mVelocity.x + mVelocity.z * mVelocity.z);
		if (triangleFunction >= 0.001f)
		{
			mDisplacementVector.x = (mVelocity.x / triangleFunction) * mSpeed;
			mDisplacementVector.z = (mVelocity.z / triangleFunction) * mSpeed;
		}
		else
		{
			mDisplacementVector.x = 0;
			mDisplacementVector.z = 0;
		}

		mDisplacementVector.y = mVelocity.y;
	}

	void CharacterMovement::CopyDirectionToPxVec3(physx::PxVec3& direction)
	{
		direction.x = mDisplacementVector.x;
		direction.y = mDisplacementVector.y;
		direction.z = -mDisplacementVector.z;
	}
}

