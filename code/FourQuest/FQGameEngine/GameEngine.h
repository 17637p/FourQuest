 #pragma once

#include <memory>

#include "IEngine.h"

namespace fq
{
	namespace game_engine
	{
		class GameProcess;

		class GameEngine :public IEngine
		{
		public:
			GameEngine();
			~GameEngine();

			void Initialize() override;
			void Process() override;
			void Finalize() override;

		private:
			std::unique_ptr<GameProcess> mGameProcess;
		};
	}
}