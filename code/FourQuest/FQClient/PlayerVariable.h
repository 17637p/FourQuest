#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class PlayerVariable : public IGameVariable
	{
	public:
		static inline float FeverAttackIncreaseRatio = 1.f;
		static inline float FeverSpeedIncreaseRatio = 1.f;
		static inline float HpReductionOnAttack = 0.f; // ���ݽ� ü�� ���� �ɼ� 
		static inline float HpReductionOnAttackMinHp = 10.f; // ���ݽ� ������ ���HP�� �ּ�Hp
		static inline float PadRotationSpeed = 4.f; // �÷��̾� �е忡 ����޴� ȸ�� �ӵ� 
		static inline float PvPDamageRatio = 1.f;

	};
}