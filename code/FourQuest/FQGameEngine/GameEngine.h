 #pragma once

#include <memory>

#include "IEngine.h"

namespace fq
{
	namespace game_engine
	{
		class GameProcess;
		class LightmapWindow;
		class LogWindow;

		class GameEngine :public IEngine
		{
		public:
			GameEngine();
			~GameEngine();

			bool Initialize() override;
			void Process() override;
			void Finalize() override;

		private:
			std::unique_ptr<GameProcess> mGameProcess;
			std::unique_ptr<LightmapWindow> mLightMap;
			std::unique_ptr<LogWindow> mLogWindow;
		};
	}
}