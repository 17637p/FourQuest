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

	class SkyBoxWindow : public IEditorWindow
	{
	public:
		SkyBoxWindow();
		~SkyBoxWindow();

		void Initialize(GameProcess* game);
		void Render() override;
		bool& IsWindowOpen() { return mbIsOpen; }

		void SaveSkyBox(std::filesystem::path path);

	private:
		bool mbIsOpen;

		fq::game_module::SkyBox mSkyBoxInfo;
		fq::graphics::IFQGraphics* mGraphicsEngine;
		fq::game_engine::GameProcess* mGameProcess;

		fq::game_module::EventHandler mOnLoadSceneHandler;
	};


}