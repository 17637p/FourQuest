#pragma once

#include "../FQGameModule/EventHandler.h"
#include "../FQGameModule/Event.h"

namespace fq::game_engine
{
	class GameProcess;

	class SysquenceSystem
	{
		using EventHandler = fq::game_module::EventHandler;
	public:
		SysquenceSystem();
		~SysquenceSystem();

		/// <summary>
		/// ������ �ý����� �ʱ�ȭ�մϴ�
		/// </summary>
		void Initialize(GameProcess* gameProcess);


	public:
		GameProcess* mGameProcess;

	};
}