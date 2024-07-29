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
		None, // �з����� ����
		Fixed, // ������������ �з���
		TargetPosition // ���� ��ġ�� �ݴ�������� �з��� 
	};

	struct AttackInfo
	{
		fq::game_module::GameObject* attacker = nullptr;
		float damage = -1.f; 
		bool bIsInfinite = true;
		unsigned int remainingAttackCount = -1;
		DirectX::SimpleMath::Vector3 attackDirection{};
		DirectX::SimpleMath::Vector3 attackPosition{}; // ������ġ

		EKnockBackType type = EKnockBackType::None;
		float knocBackPower = 0.f;
	};

}