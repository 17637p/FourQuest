#pragma once

#include <string>

namespace fq::game_engine
{
	class EditorHelper
	{
	public:
		static std::string GetStartSceneName();
		static void SetStartSceneName(std::string name);
	};
}
