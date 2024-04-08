#pragma once

#include <list>
#include <memory>

namespace fq::game_engine
{
	class Command;

	/// <summary>
	/// Editor Command¸¦ °ü¸® 
	/// </summary>
	class CommandSystem
	{
	public:
		CommandSystem();
		~CommandSystem();

		void Excute();
		void Undo();
		void Redo();
		void Clear();

	private:
		std::list<std::unique_ptr<Command>> mCommandList;
	};
}