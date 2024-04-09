#pragma once

#include <string>

namespace fq::game_module
{
	class GameObject;
}

namespace fq::event
{
	/// <summary>
	/// 게임 오브젝트 파괴시 호출
	/// </summary>
	struct OnGameObjectDestroyed
	{
		fq::game_module::GameObject* object;
	};

	/// <summary>
	/// Scene 변경시 호출
	/// </summary>
	struct OnChangeScene
	{
		std::string sceneName;
	};

}
