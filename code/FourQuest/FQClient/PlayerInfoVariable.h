#pragma once
#include "IGameVariable.h"

namespace fq::client
{
	class PlayerInfoVariable : public IGameVariable
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//					    씬 전환되어도 유지될 정보							//
		//////////////////////////////////////////////////////////////////////////

		// 플레이어 영혼 선택 종류
		static inline int Player1SoulType = -1;
		static inline int Player2SoulType = -1;
		static inline int Player3SoulType = -1;
		static inline int Player4SoulType = -1;

		// 플레이어 남은 HP
		static inline float Player1HP = 100.f;
		static inline float Player2HP = 100.f;
		static inline float Player3HP = 100.f;
		static inline float Player4HP = 100.f;

		// 플레이어 SoulGauge
		static inline float Player1SoulGauge = 100.f;
		static inline float Player2SoulGauge = 100.f;
		static inline float Player3SoulGauge = 100.f;
		static inline float Player4SoulGauge = 100.f;

		/// 결과 화면 용
		// 갑옷 사용 수 
		static inline int Player1Knight = 0;
		static inline int Player2Knight = 0;
		static inline int Player3Knight = 0;
		static inline int Player4Knight = 0;

		static inline int Player1Magic = 0;
		static inline int Player2Magic = 0;
		static inline int Player3Magic = 0;
		static inline int Player4Magic = 0;

		static inline int Player1Warrior = 0;
		static inline int Player2Warrior = 0;
		static inline int Player3Warrior = 0;
		static inline int Player4Warrior = 0;

		static inline int Player1Archer = 0;
		static inline int Player2Archer = 0;
		static inline int Player3Archer = 0;
		static inline int Player4Archer = 0;

		// 처치한 몬스터 수
		static inline int Player1Monster = 0;
		static inline int Player2Monster = 0;
		static inline int Player3Monster = 0;
		static inline int Player4Monster = 0;
	};
}
