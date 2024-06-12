#pragma once


namespace fq::game_module
{
	enum class ETag : int
	{
		Untagged = 0,
		Camera = 1,
		Player =2,
		Alien = 3,
		Obstacle = 4,
		Wall = 5,
		PlayerAttack = 6,
		MonsterAttack = 7,
		Floor = 8,
		Tag9 = 9,
		Tag10 = 10,
		Tag11 = 11,
		Tag12 = 12,
		Tag13 = 13,
		Tag14 = 14,
		Tag15 = 15,
		End =16,
	};
}