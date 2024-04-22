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
	/// ������ â�Ʒ��� �����ȴ� �޴��� 
	/// </summary>
	class MainMenuBar : public IEditorWindow
	{
		using PathType = std::filesystem::path;

	public:
		MainMenuBar();
		~MainMenuBar();

	public:
		/// <summary>
		/// MenuBar�� �ʱ�ȭ�մϴ�
		/// </summary>
		/// <param name="game">�������μ���</param>
		/// <param name="editor">���������μ���</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		void Render() override;

		/// <summary>
		/// ���� ���̸��� �����մϴ�
		/// </summary>
		/// <param name="sceneName"></param>
		void SetCurrentSceneName(std::string sceneName);

		/// <summary>
		/// ���� ���� �����մϴ�
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