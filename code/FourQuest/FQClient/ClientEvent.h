#pragma once

#include "PlayerDefine.h"
#include "../FQGameModule/GameModule.h"

namespace fq::client::event
{
	/// <summary>
	/// �÷��̾� ��� �̺�Ʈ 
	/// </summary>
	struct RegisterPlayer
	{
		game_module::GameObject* player;
		PlayerType type;
	};
}