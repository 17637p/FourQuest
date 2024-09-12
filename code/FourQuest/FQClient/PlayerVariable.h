#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class PlayerVariable : public IGameVariable
	{
	public:
		static inline float FeverAttackIncreaseRatio = 1.5f;
		static inline float FeverSpeedIncreaseRatio = 2.f;
	};

}