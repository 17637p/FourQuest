#pragma once

#include <directxtk\SimpleMath.h>

#include "FQPhysics.h"

namespace fq::physics
{
	class CharaterMovement
	{
	public:
		CharaterMovement();
		~CharaterMovement();

		void initialize(const CharacterMovementInfo& info);
		void Update(float deltaTime, const DirectX::SimpleMath::Vector3& input);

	private:
		bool mIsFall;

		DirectX::SimpleMath::Vector3 mVelocity;
		DirectX::SimpleMath::Vector3 mDirection;

		float mMaxSpeed;
		float mSpeed;
		float mAcceleration;
		float mStaticFriction;
		float mDynamicFriction;
		float mJumpSpeed;
		float mJumpXZAcceleration;
		float mJumpXZDeceleration;
		float mGravity;
		float mMinDistance;
	};
}