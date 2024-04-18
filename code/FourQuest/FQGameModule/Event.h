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
	/// Scene Load 할때 호출
	/// </summary>
	struct OnLoadScene
	{
		std::string sceneName;
	};
	
	/// <summary>
	/// Scene Unload할 때 호출
	/// </summary>
	struct OnUnloadScene
	{
	};

	/// <summary>
 	/// 씬 변경 요청 이벤트
	/// </summary>
	struct RequestChangeScene
	{
		std::string sceneName;
	};

	/// <summary>
	/// 게임 종료 요청 이벤트 
	/// </summary>
	struct RequestExitGame 
	{
	};

	/// <summary>
	/// 씬에 새로운 게임오브젝트를 추가시 호출
	/// </summary>
	struct AddGameObject 
	{
		fq::game_module::GameObject* object;
	};


}
