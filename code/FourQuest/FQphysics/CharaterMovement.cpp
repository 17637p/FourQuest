#include "CharaterMovement.h"

namespace fq::physics
{
	CharaterMovement::CharaterMovement()
		: mMaxSpeed()
		, mSpeed()
		, mAcceleration()
		, mStaticFriction()
		, mDynamicFriction()
		, mJumpSpeed()
		, mJumpXZAcceleration()
		, mJumpXZDeceleration()
		, mGravity()
		, mMinDistance()
		, mIsFall()
		, mVelocity()
		, mDirection()
	{

	}

	CharaterMovement::~CharaterMovement()
	{

	}

	void CharaterMovement::initialize(const CharacterMovementInfo& info)
	{
		mMaxSpeed = info.maxSpeed;
		mAcceleration = info.acceleration;
		mStaticFriction = info.staticFriction;
		mDynamicFriction = info.dynamicFriction;
		mJumpSpeed = info.jumpSpeed;
		mJumpXZAcceleration = info.jumpXZAcceleration;
		mJumpXZDeceleration = info.jumpXZDeceleration;
		mGravity = info.gravity;
		mMinDistance = 0.1f;
	}

	void CharaterMovement::Update(float deltaTime, const DirectX::SimpleMath::Vector3& input)
	{

	}
}

