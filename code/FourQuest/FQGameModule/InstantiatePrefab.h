#pragma once

#include <memory>
#include <vector>

#include "PrefabObject.h"
#include "GameObject.h"

namespace fq::event
{
	struct InstantiatePrefab
	{
		fq::game_module::PrefabObject instantiatePrefabObject;
		std::vector<std::shared_ptr<fq::game_module::GameObject>>& instance;
	};
}