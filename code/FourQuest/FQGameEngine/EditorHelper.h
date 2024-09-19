#pragma once

#include <string>

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class EditorHelper
	{
	public:
		static std::string GetStartSceneName();
		static void SetStartSceneName(std::string name);

		static void UpdateEditorMode(GameProcess* game, float dt);
		static void LoadEditorSetting(EditorProcess* editor);
		static void SaveEditorSetting(EditorProcess* editor);

		inline static std::string EditorVersion = "0.7.0";
	};
}
