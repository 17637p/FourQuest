#pragma once

namespace fq::client
{
	/// <summary>
	/// 몬스터 타입
	/// 이름이 이게뭐야!
	/// </summary>
	enum class EMonsterType
	{
		Melee, // 근접 몬스터
		Explosion, // 폭탄 몬스터
		Boss, // 보스
		Plant, // 식물 몬스터
		Spawner, // 스포너
		All // Quest 용 
	};

	enum class EBossMonsterAttackType
	{
		Rush = 0, // 러쉬 
		SmashDown = 1, // 내려찍기
		Combo = 2, // 콤보 공격
		Roar = 3, // 로어 
		Continous =4, // 연속공격
		Eat = 5, // 먹기 
	};
}
