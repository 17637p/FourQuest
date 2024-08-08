#pragma once

#include <vector>
#include <string>
#include <set>

namespace fq::game_engine
{
	struct SceneResourceList
	{
		using Path = std::string;

		std::set<Path> modelPaths;
		std::set<Path> materialPaths;
		std::set<Path> animatorControllerPaths;
		std::set<Path> animationPaths;

		void Load(const Path& path);
		void Save(const Path& path);
	};
}

