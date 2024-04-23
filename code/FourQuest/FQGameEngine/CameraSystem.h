#pragma once


namespace fq::game_engine
{
	class GameProcess;

	class CameraSystem
	{
	public:
		CameraSystem();
		~CameraSystem();

		void Initialize(GameProcess* gameProcess);

	private:
		GameProcess* mGameProcess;
	};

}
