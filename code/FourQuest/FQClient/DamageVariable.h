#pragma once
#include "IGameVariable.h"


namespace fq::client
{
	class DamageVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//							검 갑옷 공격 배율								//
		//////////////////////////////////////////////////////////////////////////
	
		static inline float SwordAttackCoefficient = 1.f;
		static inline float ShieldAttackCoefficient = 1.f;
		static inline float ShieldDashCoefficient = 1.f;
		
		//////////////////////////////////////////////////////////////////////////
		//						마법 갑옷 공격 배율								//
		//////////////////////////////////////////////////////////////////////////
		static inline float MagicBallCoefficient = 1.f;
		static inline float MagicAOECoefficient = 1.f;
		static inline float MagicLaserCoefficient = 1.f;

		//////////////////////////////////////////////////////////////////////////
		//							영혼 공격 배율								//
		//////////////////////////////////////////////////////////////////////////
		static inline float SwordSoulCoefficient = 1.f;
		static inline float StaffSoulCoefficient = 1.f;
		static inline float AxeSoulCoefficient = 1.f;
		static inline float BowSoulCoefficient = 1.f;

		//////////////////////////////////////////////////////////////////////////
		//							보스 공격 배율								//
		//////////////////////////////////////////////////////////////////////////
		static inline float BossSmashDownCoefficient = 1.f;
		static inline float BossComboAttackCoefficient = 1.f;
		static inline float BossRushCoefficient = 1.f;


	};


}
