#pragma once

#include <directxtk/SimpleMath.h>

namespace fq::game_module
{
	class GameObject;
}


namespace fq::client
{
	enum class EKnockBackType
	{
		None, // 밀려나지 않음
		Fixed, // 고정방향으로 밀려남
		TargetPosition // 공격 위치의 반대방향으로 밀려남 
	};

	struct AttackInfo
	{
		fq::game_module::GameObject* attacker = nullptr;
		float damage = -1.f; 
		bool bIsInfinite = true;
		unsigned int remainingAttackCount = -1;
		DirectX::SimpleMath::Vector3 attackDirection{};
		DirectX::SimpleMath::Vector3 attackPosition{}; // 공격위치

		EKnockBackType type = EKnockBackType::None;
		float knocBackPower = 0.f;
		
		std::string hitSound; // 피격 소리 
		std::function<void()> mHitCallback; // 공격성공 콜백함수
	};

	struct ArrowAttackInfo
	{
		fq::game_module::GameObject* attacker = nullptr;

		bool bIsStrongAttack = false;
		float weakDamage = -1.f;
		float strongDamage = -1.f;
		int remainingAttackCount = -1;
		DirectX::SimpleMath::Vector3 attackDirection{};
		DirectX::SimpleMath::Matrix attackTransform{};

		std::string hitSound; // 피격 소리
	};

}