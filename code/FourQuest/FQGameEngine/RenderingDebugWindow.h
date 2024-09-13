#pragma once

#include "IEditorWindow.h"

#include <filesystem>

#include "../FQGameModule/GameModule.h"

namespace fq::graphics
{
	class IFQGraphics;
}

namespace fq::game_engine
{
	class GameProcess;

	class RenderingDebugWindow : public IEditorWindow
	{
	public:
		RenderingDebugWindow();
		~RenderingDebugWindow();

		void Initialize(GameProcess* game);
		void Render() override;
		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		bool mbIsOpen;

		fq::graphics::IFQGraphics* mGraphicsEngine;
		fq::game_engine::GameProcess* mGameProcess;
	};
}