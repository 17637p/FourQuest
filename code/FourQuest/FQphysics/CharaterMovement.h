#pragma once

#include <directxtk\SimpleMath.h>

namespace fq::physics
{
	class CharaterMovement
	{
	public:
		CharaterMovement();
		~CharaterMovement();

		void initialize();


	private:
		bool mIsFall;

		DirectX::SimpleMath::Vector3 m_Velocity;
		DirectX::SimpleMath::Vector3 m_Direction;

		float m_MaxSpeed;
		float m_Speed;
		float m_Acceleration;
		float m_StaticFriction;
		float m_DynamicFriction;
		float m_JumpSpeed;
		float m_JumpXZAcceleration;
		float m_JumpXZDeceleration;
		float m_Gravity;
		float m_MinDistance;
	};
}