#pragma once

#include <memory>
#include <vector>

#include "PrefabResource.h"
#include "GameObject.h"

namespace fq::event
{
	struct InstantiatePrefab
	{
		fq::game_module::PrefabResource instantiatePrefabObject;
		std::vector<std::shared_ptr<fq::game_module::GameObject>>& instance;
	};
}