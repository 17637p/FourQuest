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

	class LightmapWindow : public IEditorWindow
	{
	public:
		LightmapWindow();
		~LightmapWindow();

		void Initialize(GameProcess* game);
		void Render() override;

		void SaveLightmap(std::filesystem::path path);
		void LoadLightmap(std::filesystem::path path);
		void ApplyLightmap();

		inline bool& IsWindowOpen();

	private:
		bool mbIsOpen;

		fq::game_module::SkyBox mSkyBoxInfo;

		std::vector<std::filesystem::path> mLightmapArrayPath;
		std::vector<std::filesystem::path> mDirectionArrayPath;

		fq::graphics::IFQGraphics* mGraphicsEngine;
		fq::game_engine::GameProcess* mGameProcess;

		fq::game_module::EventHandler mOnLoadSceneHandler;
	};

	bool& LightmapWindow::IsWindowOpen()
	{
		return mbIsOpen;
	}
}