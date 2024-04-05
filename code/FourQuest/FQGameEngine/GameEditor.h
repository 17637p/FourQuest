#pragma once

#include "IEngine.h"

namespace fq
{
	namespace game_engine
	{
		class GameEditor : public IEngine
		{
		public:
			GameEditor();
			~GameEditor();

			void Initialize(HINSTANCE hInstance) override;
			void Process() override;
			void Finalize() override;

		private:
			void initializeWindow(HINSTANCE hInstance);

		private:
			HWND mHWND;

		};
	}
}

