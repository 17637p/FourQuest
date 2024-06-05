#pragma once
#include <memory>
#include <string>
#include <unordered_map>

namespace fq::client
{
	class MonsterBaseState;

	class MonsterFSM
	{
	public:
		MonsterFSM();
		~MonsterFSM();

		void RegisterState(const std::string stateName, const std::shared_ptr<MonsterBaseState>& state);
		void DeleteState(const std::string stateName);

		void OnStart(const std::string& baseState);
		void OnChangeState(const std::string& nextState);
		void OnUpdate(float deltaTime);

	private:
		std::string mCurState;
		std::unordered_map<std::string, std::shared_ptr<MonsterBaseState>> mStates;
	};
}

