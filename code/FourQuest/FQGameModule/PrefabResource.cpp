#include "PrefabResource.h"

#include <filesystem>
#include <spdlog/spdlog.h>

fq::game_module::PrefabResource::PrefabResource(const Path& string)
	: mPrefabPath(string)
{
}

void fq::game_module::PrefabResource::SetPrefabPath(Path path)
{
	mPrefabPath = path;
}
