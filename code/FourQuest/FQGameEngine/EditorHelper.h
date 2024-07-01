#pragma once

#include <string>

namespace fq::game_engine
{
	class GameProcess;

	class EditorHelper
	{
	public:
		static std::string GetStartSceneName();
		static void SetStartSceneName(std::string name);

		static void UpdateEditorMode(GameProcess* game, float dt);
	};
}
