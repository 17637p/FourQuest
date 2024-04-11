#pragma once

#include <memory>

#include "../FQGameModule/GameModule.h"

namespace fq::game_engine::editor_event
{
	struct SelectObject
	{	
		 std::shared_ptr<fq::game_module::GameObject> object;
	};



}