#pragma once

#include <directxtk/SimpleMath.h>
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
		static inline float SoulBowAttackDuration = 2.f;

		static inline float SoulAxeAttackTick = 1.f;
		static inline float SoulAxeAttackDuration = 2.f;

		static inline float SoulSwordAttackCost = 50.f; 
		static inline float SoulBowAttackCost = 50.f;
		static inline float SoulAxeAttackCost = 50.f;
		static inline float SoulStaffAttackCost = 50.f;
		static inline float SoulGaugeCharging = 10.f; // 소울게이지 차징량 


		static inline DirectX::SimpleMath::Color SwordSoulColor;
		static inline DirectX::SimpleMath::Color StaffSoulColor;
		static inline DirectX::SimpleMath::Color AxeSoulColor;
		static inline DirectX::SimpleMath::Color BowSoulColor;
	};

}