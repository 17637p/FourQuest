#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class GameProcess;

	class AnimationSystem
	{
	public:
		AnimationSystem();
		~AnimationSystem();

		/// <summary>
		/// 애니메이션 시스템 초기화
		/// </summary>
		void Initialize(GameProcess* game);

	public:
		GameProcess* mGameProcess;
	};


}