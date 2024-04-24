#pragma once

#include "../FQGameModule/GameModule.h"

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
		fq::game_module::EventHandler mSetMainCameraHandler;
		fq::game_module::Camera* mMainCamera;
	};

}
