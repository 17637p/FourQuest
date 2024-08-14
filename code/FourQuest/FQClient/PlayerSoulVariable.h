#pragma once

#include "IGameVariable.h"

namespace fq::client
{
	class PlayerSoulVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//							영혼 공격 관련 								// 
		//////////////////////////////////////////////////////////////////////////
		
		static inline float SoulBowAttackTick = 0.1f;
		static inline float SoulBowAttackSpeed = 5.f;
		static inline float SoulBowAttackAngle = 20.f;
		static inline float SoulBowAttackOffset = 1.5f;

		static inline float SoulAxeAttackTick = 1.f;
	};

}