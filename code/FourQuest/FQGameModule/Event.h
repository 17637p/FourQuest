#pragma once

#include <string>

namespace fq::game_module
{
	class GameObject;
}

namespace fq::event
{
	/// <summary>
	/// ���� ������Ʈ �ı��� ȣ��
	/// </summary>
	struct OnGameObjectDestroyed
	{
		fq::game_module::GameObject* object;
	};

	/// <summary>
	/// Scene Load �Ҷ� ȣ��
	/// </summary>
	struct OnLoadScene
	{
		std::string sceneName;
	};
	
	/// <summary>
	/// Scene Unload�� �� ȣ��
	/// </summary>
	struct OnUnloadScene
	{
	};

	/// <summary>
 	/// �� ���� ��û �̺�Ʈ
	/// </summary>
	struct RequestChangeScene
	{
		std::string sceneName;
	};

	/// <summary>
	/// ���� ���� ��û �̺�Ʈ 
	/// </summary>
	struct RequestExitGame 
	{
	};

	/// <summary>
	/// ���� ���ο� ���ӿ�����Ʈ�� �߰��� ȣ��
	/// </summary>
	struct AddGameObject 
	{
		fq::game_module::GameObject* object;
	};


}
