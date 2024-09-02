#pragma once


namespace fq::game_module
{
	enum class ETag : int
	{
		Untagged = 0,
		Camera = 1,
		Player =2,
		Monster = 3,
		Obstacle = 4,
		Wall = 5,
		PlayerAttack = 6,
		MonsterAttack = 7,
		Armour = 8,
		Floor = 9,
		Soul = 10,
		Dash = 11,
		Spawner = 12,
		Goddess = 13,
		Box = 14,
		Tag15 = 15,
		End =16,
	};
}