#include "RenderingSystem.h"

#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

fq::game_engine::RenderingSystem::RenderingSystem()
	:mGameProcess(nullptr)
{}

fq::game_engine::RenderingSystem::~RenderingSystem()
{}

void fq::game_engine::RenderingSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle ���

	auto& eventMgr = mGameProcess->mEventManager;

	eventMgr->RegisterHandle<fq::event::OnLoadScene>(this, &RenderingSystem::LoadScene);
	eventMgr->RegisterHandle<fq::event::OnUnloadScene>(this, &RenderingSystem::UnLoadScene);

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

void fq::game_engine::RenderingSystem::LoadScene()
{
	// 1. Scene�� ��ġ�� ���Ҹ��� �ε� 
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	// 2. PrefabInstance�� �ε�

}

void fq::game_engine::RenderingSystem::UnLoadScene()
{
	// 1. Model Unload
	

	// 2. MeshObject Unload

}
