#include "PrefabResource.h"

#include <filesystem>
#include <spdlog/spdlog.h>

void fq::game_module::PrefabResource::SetPrefabPath(Path path)
{
	mPrefabPath = path;
}
