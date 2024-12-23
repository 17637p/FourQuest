#pragma once
#include <string>
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
		static inline int Player1SoulType = -1; // Esoultype 0 검, 1 마법, 2 도끼, 3 활
		static inline int Player2SoulType = -1;
		static inline int Player3SoulType = -1;
		static inline int Player4SoulType = -1;

		// 플레이어 영혼 이름
		static inline std::string KnightName = "루카스";
		static inline std::string MagicName = "아르카나";
		static inline std::string ArcherName = "실버";
		static inline std::string BerserkerName = "발더";

		// 플레이어 현재 상태 
		static inline int Player1State = -1; // 0 영혼, 1 검방, 2 마법, 3 궁수, 4 광전사
		static inline int Player2State = -1; 
		static inline int Player3State = -1; 
		static inline int Player4State = -1; 

		// 플레이어 스폰 위치
		static inline std::string Player1SpawnPosObject = "PlayerSpawner1";
		static inline std::string Player2SpawnPosObject = "PlayerSpawner2";
		static inline std::string Player3SpawnPosObject = "PlayerSpawner3";
		static inline std::string Player4SpawnPosObject = "PlayerSpawner4";

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
