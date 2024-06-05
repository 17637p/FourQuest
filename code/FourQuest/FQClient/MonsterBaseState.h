#pragma once
#include <memory>

namespace fq::client
{
	class Monster;

	class MonsterBaseState
	{
	public:
		virtual void OnStateEnter() abstract;
		virtual void OnStateUpdate(float deltaTime) abstract;
		virtual void OnStateExit() abstract;

		virtual ~MonsterBaseState() {}

	protected:
		MonsterBaseState(std::shared_ptr<Monster> monster) : mMonster(monster) {};

	protected:
		std::shared_ptr<Monster> mMonster;
	};
}

