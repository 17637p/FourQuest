#pragma once

#include <vector>
#include <string>
#include <set>

namespace fq::game_engine
{
	/// <summary>
	/// ���� �ʿ��� ���ҽ� ����Ʈ 
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

