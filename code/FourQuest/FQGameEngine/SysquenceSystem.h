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
		/// 랜더링 시스템을 초기화합니다
		/// </summary>
		void Initialize(GameProcess* gameProcess);


	public:
		GameProcess* mGameProcess;

	};
}