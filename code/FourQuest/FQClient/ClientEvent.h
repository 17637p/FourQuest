#pragma once

#include "PlayerDefine.h"
#include "MonsterDefine.h"

#include "../FQGameModule/GameModule.h"

namespace fq::client::event
{
	/// <summary>
	/// 플레이어 등록 이벤트 
	/// </summary>
	struct RegisterPlayer
	{
		game_module::GameObject* player;
		EPlayerType type;
	};

	/// <summary>
	/// 몬스터를 죽였을 때 발생하는 이벤트 
	/// </summary>
	struct KillMonster
	{
		EMonsterType monsterType;
	};

	/// <summary>
	/// 플레이어가 트리거에 부딪힐 때마다 발생하는 이벤트
	/// </summary>
	struct PlayerCollideTrigger
	{
		int playerNumber;
		std::string colliderName;
	};
}