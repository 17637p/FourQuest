#pragma once

#include "IEditorWindow.h"

#include <imgui.h>
#include <filesystem>
#include <fstream>

namespace fq::game_module
{
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class ExportWindow : public IEditorWindow
	{
	public:
		ExportWindow();
		~ExportWindow();

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Render() override;

		void SaveGameObjectsForUnity();

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void PrintGameObject(fq::game_module::GameObject* object);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		std::filesystem::path mDirPath; // "./resource/Export/" °æ·Î 
		std::ofstream mOutStream;

		bool mbIsOpen;
	};
}

