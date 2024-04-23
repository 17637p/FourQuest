#pragma once

namespace fq::game_engine
{
	class GameProcess;

	class RenderingSystem
	{
	public:
		RenderingSystem();
		~RenderingSystem();

		void Initialize(GameProcess* gameProcess);

		void Update(float dt);

	private:
		GameProcess* mGameProcess;
	};
}