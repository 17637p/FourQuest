#pragma once

#include <memory>

namespace fq::game_module
{
	class GameObject;
}

namespace fq::client
{
	/// <summary>
	/// ���� �������̽� 
	/// </summary>
	class IMonster
	{
	public:
		virtual ~IMonster(){}

		virtual void SetTarget(game_module::GameObject* target) abstract;
	};
}
