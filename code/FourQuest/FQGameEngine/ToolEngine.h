#pragma once

#include <memory>

#include "IEngine.h"

namespace fq
{
	namespace game_engine
	{
		class GameProcess;
		class Editor;

		class ToolEngine : public IEngine
		{
		public:
			ToolEngine();
			~ToolEngine();

			void Initialize() override;
			void Process() override;
			void Finalize() override;

		private:
			std::unique_ptr<GameProcess> mGameProcess;
			std::unique_ptr<Editor> mEditor;
		};
	}
}

