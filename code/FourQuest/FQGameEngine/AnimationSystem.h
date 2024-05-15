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
		/// �ִϸ��̼� �ý��� �ʱ�ȭ
		/// </summary>
		void Initialize(GameProcess* game);

	public:
		GameProcess* mGameProcess;
	};


}