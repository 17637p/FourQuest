//#pragma once
//
//#include "IEditorWindow.h"
//
//#include <filesystem>
//
//#include "../FQGameModule/GameModule.h"
//
//namespace fq::graphics
//{
//	class IFQGraphics;
//}
//
//namespace fq::game_engine
//{
//	class GameProcess;
//
//	class LightmapWindow : public IEditorWindow
//	{
//	public:
//		LightmapWindow();
//		~LightmapWindow();
//
//		void Initialize(GameProcess* game);
//		void Render() override;
//		bool& IsWindowOpen() { return mbIsOpen; }
//
//		void SaveLightmap(std::filesystem::path path);
//
//	private:
//		bool mbIsOpen;
//
//		fq::game_module::SkyBox mSkyBoxInfo;
//
//		std::vector<std::string> mLightmapArrayPath;
//		std::vector<std::string> mLightmapDirectionArrayPath;
//
//		fq::graphics::IFQGraphics* mGraphicsEngine;
//		fq::game_engine::GameProcess* mGameProcess;
//
//		fq::game_module::EventHandler mOnLoadSceneHandler;
//	};
//}