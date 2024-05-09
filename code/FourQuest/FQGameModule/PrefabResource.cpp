#include "PrefabResource.h"

#include <filesystem>
#include <spdlog/spdlog.h>

void fq::game_module::PrefabResource::SetPrefabPath(Path path)
{
	if (!std::filesystem::exists(path))
	{
		spdlog::warn("[PrefabResource] {} not exist", path);
		mPrefabPath.clear();
	}
	else
	{
		mPrefabPath = path;
	}
}
