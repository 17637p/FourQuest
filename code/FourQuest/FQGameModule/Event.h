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
	/// Scene ����� ȣ��
	/// </summary>
	struct OnChangeScene
	{
		std::string sceneName;
	};

	/// <summary>
	/// ���� ���ο� ���ӿ�����Ʈ�� �߰��� ȣ��
	/// </summary>
	struct AddGameObject 
	{
		fq::game_module::GameObject* object;
	};

}
