#include "LightSystem.h"

#include "GameProcess.h"

fq::game_engine::LightSystem::LightSystem()
	:mGameProcess(nullptr)
	,mbIsGameLoaded(false)
{}

fq::game_engine::LightSystem::~LightSystem()
{}

void fq::game_engine::LightSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;
}

void fq::game_engine::LightSystem::OnLoadScene(const fq::event::OnLoadScene event)
{
}

void fq::game_engine::LightSystem::OnUnLoadScene()
{

}

void fq::game_engine::LightSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{

}

void fq::game_engine::LightSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{

}
