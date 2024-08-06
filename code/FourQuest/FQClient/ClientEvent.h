#pragma once

#include "PlayerDefine.h"
#include "MonsterDefine.h"

#include "../FQGameModule/GameModule.h"

namespace fq::client::event
{
	/// <summary>
	/// �÷��̾� ��� �̺�Ʈ 
	/// </summary>
	struct RegisterPlayer
	{
		game_module::GameObject* player;
		EPlayerType type;
	};

	/// <summary>
	/// ���͸� �׿��� �� �߻��ϴ� �̺�Ʈ 
	/// </summary>
	struct KillMonster
	{
		EMonsterType monsterType;
	};

	/// <summary>
	/// �÷��̾ Ʈ���ſ� �ε��� ������ �߻��ϴ� �̺�Ʈ
	/// </summary>
	struct PlayerCollideTrigger
	{
		int playerNumber;
		std::string colliderName;
	};
}