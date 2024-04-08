#include "SceneManager.h"
#include "Scene.h"

fq::game_module::SceneManager::SceneManager()
	:mCurrentScene(nullptr)
{
}

fq::game_module::SceneManager::~SceneManager()
{
}

void fq::game_module::SceneManager::Initialize()
{
	mCurrentScene = std::make_unique<Scene>();
}

void fq::game_module::SceneManager::Finalize()
{

}
