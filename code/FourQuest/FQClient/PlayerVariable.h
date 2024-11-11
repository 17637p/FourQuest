#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class PlayerVariable : public IGameVariable
	{
	public:
		static inline float FeverAttackIncreaseRatio = 1.f;
		static inline float FeverSpeedIncreaseRatio = 1.f;
		static inline float HpReductionOnAttack = 0.f; // 공격시 체력 감소 옵션 
		static inline float HpReductionOnAttackMinHp = 10.f; // 공격시 감소한 결과HP의 최소Hp
		static inline float PadRotationSpeed = 4.f; // 플레이어 패드에 영향받는 회전 속도 
		static inline float PvPDamageRatio = 1.f;

	};
}