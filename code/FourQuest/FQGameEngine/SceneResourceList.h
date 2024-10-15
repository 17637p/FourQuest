#pragma once

#include <vector>
#include <string>
#include <set>

namespace fq::game_engine
{
	/// <summary>
	/// 씬에 필요한 리소스 리스트 
	/// </summary>
	struct SceneResourceList
	{
		using Path = std::string;

		std::set<Path> modelPaths;
		std::set<Path> materialPaths;
		std::set<Path> animatorControllerPaths;
		std::set<Path> uvAnimationPath;
		std::set<Path> animationPaths;
		std::set<Path> nodeHierachyPaths;

		void Load(const Path& path);
		void Save(const Path& path);
	};
}

