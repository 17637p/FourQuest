#include "FQGameEnginePCH.h"
#include "GameProcess.h"

#include "WindowSystem.h"

fq::game_engine::GameProcess::GameProcess()
	:mWindowSystem(std::make_unique<WindowSystem>())
{

}

fq::game_engine::GameProcess::~GameProcess()
{

}

