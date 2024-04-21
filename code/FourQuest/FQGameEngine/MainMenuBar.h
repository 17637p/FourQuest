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
	class MainMenuBar : public IEditorWindow
	{
		using PathType = std::filesystem::path;

	public:
		MainMenuBar();
		~MainMenuBar();

	public:
		/// <summary>
		/// MenuBar를 초기화합니다
		/// </summary>
		/// <param name="game">게임프로세스</param>
		/// <param name="editor">에디터프로세스</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		void Render() override;

		/// <summary>
		/// 현재 씬이름을 변경합니다
		/// </summary>
		/// <param name="sceneName"></param>
		void SetCurrentSceneName(std::string sceneName);

		/// <summary>
		/// 현재 씬을 저장합니다
		/// </summary>
		void SaveScene();
	private:
		void beginMenu_File();
		void beginMenuItem_CreateScene();
		void beginMenuItem_LoadScene();
		void beginMenuItem_SaveScene();

		void beginOpenPopup_CreateScene();

		void beginText_SceneName();
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