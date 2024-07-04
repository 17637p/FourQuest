#pragma once

#include "PlayerDefine.h"
#include "../FQGameModule/GameModule.h"

namespace fq::client::event
{
	/// <summary>
	/// 플레이어 등록 이벤트 
	/// </summary>
	struct RegisterPlayer
	{
		game_module::GameObject* player;
		PlayerType type;
	};
}