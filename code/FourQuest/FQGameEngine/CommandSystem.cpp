
#include "CommandSystem.h"
#include "ICommand.h"

fq::game_engine::CommandSystem::CommandSystem()
	:mCommandList{}
{}

fq::game_engine::CommandSystem::~CommandSystem()
{
	Clear();
}

void fq::game_engine::CommandSystem::Excute()
{

}

void fq::game_engine::CommandSystem::Undo()
{

}

void fq::game_engine::CommandSystem::Redo()
{

}

void fq::game_engine::CommandSystem::Clear()
{
	mCommandList.clear();
}

