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
		spawner, // 스포너

		All // Quest 용 
	};
}
