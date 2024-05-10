#pragma once

#include "IEditorWindow.h"

namespace fq::graphics
{
	class IFQGraphics;
}

namespace fq::game_engine
{
	class GameProcess;

	class SkyBoxWindow : public IEditorWindow
	{
	public:
		SkyBoxWindow();
		~SkyBoxWindow();

		void Initialize(GameProcess* game);
		void Render() override;
		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		bool mbIsOpen;
		fq::graphics::IFQGraphics* mGraphicsEngine;
	};


}