#pragma once

#include "IGameVariable.h"

namespace fq::client
{
	/// <summary>
	/// 난이도 관련 
	/// </summary>
	class LevelVariable : public IGameVariable
	{
	public:
		static inline bool OnLevelSystem = true;
		// Level
		static inline float EasyDamage = 0.f;
		static inline float EasyHp = 0.f;
		static inline float NormalDamage = 0.f;
		static inline float NormalHp = 0.f;
		static inline float HardDamage = 0.f;
		static inline float HardHp = 0.f;

		static inline float Player2Damage = 0.f;
		static inline float Player2Hp = 0.f;
		static inline float Player2Spawn = 0.f;
		
		static inline float Player3Damage = 0.f;
		static inline float Player3Hp = 0.f;
		static inline float Player3Spawn = 0.f;
		
		static inline float Player4Damage = 0.f;
		static inline float Player4Spawn = 0.f;
		static inline float Player4Hp = 0.f;
	};
}
