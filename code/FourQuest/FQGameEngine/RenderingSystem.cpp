#include "RenderingSystem.h"

#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

fq::game_engine::RenderingSystem::RenderingSystem()
	:mGameProcess(nullptr)
{

}

fq::game_engine::RenderingSystem::~RenderingSystem()
{

}

void fq::game_engine::RenderingSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;
}

void fq::game_engine::RenderingSystem::Update(float dt)
{
	using namespace fq::game_module;

	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<Transform, StaticMeshRenderer>
		([](GameObject& object, Transform& transform, StaticMeshRenderer& mesh)
			{
				auto meshObject = mesh.GetStaticMeshObject();
				if (meshObject)
				{
					meshObject->UpdateTransform(transform.GetWorldMatrix());
				}
			});

}
