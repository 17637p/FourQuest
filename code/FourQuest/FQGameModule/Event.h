#pragma once

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

}
