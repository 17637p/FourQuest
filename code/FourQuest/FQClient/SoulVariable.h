#pragma once

#include "IGameVariable.h"

namespace fq::client
{
	class SoulVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//							영혼 이탈 및 파괴								//
		//////////////////////////////////////////////////////////////////////////
		static inline float ButtonTime = 0.5f;		// 영혼 이탈 시간
		static inline float OutTime = 3.f;			// 갑옷 파괴 시, 게임 아웃 시간
		static inline int HpPercent = 30;			// 갑옷 입고 있는 상태에서 이탈할 시에 갑옷 생성할 수 있는 최소 HP

		//////////////////////////////////////////////////////////////////////////
		//								영혼 체력									//
		//////////////////////////////////////////////////////////////////////////
		static inline int SoulMaxHp = 100;				// 소울의 HP 최대치
		static inline float SoulHpDecreas = 5.f;		// 초당 HP가 감소하는 양
		static inline float SoulDistance = 5.f;			// 이 사정거리 안에 갑옷을 입은 플레이어가 있을 경우 피해량 감소
		static inline int SoulDecreasPercentage = 50;	// HP 피해 감소량
		static inline int SoulHpDown = 10;				// 영혼 파괴 시, 최대 체력 감소량
		static inline int SoulMinHp = 20;				// 사망 시 영혼 최대 체력이 줄어드는데, 일정 사망 횟수 이상에서도 최소 HP값에 도달하면 최대 HP값이 줄어들지 않는 변수
		static inline int SoulLessHp = 20;				// 갑옷이 파괴될 때마다 최대 체력이 깎이는 값

		//////////////////////////////////////////////////////////////////////////
		//								플레이어 강화								//
		//////////////////////////////////////////////////////////////////////////
		static inline float SpeedUp = 1.2f;				// 플레이어 이동속도 증가량
		static inline float DamageUp = 1.2f;			// 플레이어 공격력 증가량
	};
}