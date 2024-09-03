#pragma once

#include "IGameVariable.h"

namespace fq::client
{
	class MeleeMonsterVariable : public IGameVariable
	{
	public:
		static inline float Hp = 100.f;
		static inline float AttackPower = 100.f;
		static inline float MoveSpeed = 100.f;
	};

}
