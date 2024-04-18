#pragma once

#include <string>
#include <filesystem>

#include "IEditorWindow.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// 윈도우 창아래에 생성된는 메뉴바 
	/// </summary>
	class MenuBar : public IEditorWindow
	{
		using PathType = std::filesystem::path;

	public:
		MenuBar();
		~MenuBar();

	public:
		void Initialize(GameProcess* game, EditorProcess* editor);

		void Render() override;

		void SetCurrentSceneName(std::string sceneName);

	private:
		void beginMenu_File();
		void beginMenuItem_CreateScene();
		void beginMenuItem_LoadScene();
		void beginMenuItem_SaveScene();

		void beginOpenPopup_CreateScene();

		void beginText_FPS();

		void updateSceneList();

		void createScene(std::string sceneName);
	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		std::vector<PathType> mSceneList;

		std::string mCurrentSceneName;
		std::string mCreateSceneName;
		bool mbIsOpenCreateWindowPopup;

		fq::game_module::EventHandler mOnLoadSceneHandler;
	};



}