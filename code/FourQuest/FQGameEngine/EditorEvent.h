#pragma once

#include <memory>
#include <filesystem>

#include "../FQGameModule/GameModule.h"

namespace fq::editor_event
{
	struct SelectObject
	{	
		 std::shared_ptr<fq::game_module::GameObject> object;
	};

	struct SelectAnimationController
	{
		std::shared_ptr<fq::game_module::AnimatorController> controller;
		std::string stateName;
	};

}