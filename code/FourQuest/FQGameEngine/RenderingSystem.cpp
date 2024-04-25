#include "RenderingSystem.h"

#include <filesystem>
#include <spdlog/spdlog.h>

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

fq::game_engine::RenderingSystem::RenderingSystem()
	:mGameProcess(nullptr)
	, mOnAddGameObjectHandler{}
	, mOnLoadSceneHandler{}
	, mOnUnloadSceneHandler{}
	, mDestroyedGameObjectHandler{}
	, mbIsGameLoaded(false)
{}

fq::game_engine::RenderingSystem::~RenderingSystem()
{}

void fq::game_engine::RenderingSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle ���
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &RenderingSystem::OnLoadScene);

	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &RenderingSystem::OnUnLoadScene);

	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &RenderingSystem::OnAddGameObject);

	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &RenderingSystem::OnDestroyedGameObject);

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

void fq::game_engine::RenderingSystem::OnLoadScene()
{
	// 1. Scene�� ��ġ�� ���Ҹ��� �ε� 
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto& object : scene->GetObjectView(true))
	{
		loadStaticMeshRenderer(&object);
	}

	// 2. PrefabInstance�� �ε�

	mbIsGameLoaded = true;
}

void fq::game_engine::RenderingSystem::OnUnLoadScene()
{
	// 1. Model Unload
	unloadAllModel();

	mbIsGameLoaded = false;
}

void fq::game_engine::RenderingSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	// ���� �ε��� ���������� ó���մϴ�
	if (!mbIsGameLoaded)
	{
		return;
	}
	auto gameObject = event.object;

	// ������Ʈ�� �ʿ��� ���ҽ��� �ε��մϴ�

	// 1. StaticMesh 
	loadStaticMeshRenderer(gameObject);
}

void fq::game_engine::RenderingSystem::loadStaticMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::StaticMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::StaticMeshRenderer>();
	auto meshInfo = staticMeshRenderer->GetMeshObjectInfomation();
	auto transform = object->GetComponent<fq::game_module::Transform>();

	// Model ����
	if (!std::filesystem::exists(meshInfo.ModelPath))
	{
		SPDLOG_WARN("\"{}\" does not exist", meshInfo.ModelPath);
	}
	else
	{
		loadModel(meshInfo.ModelPath);
	}
	meshInfo.Transform = transform->GetLocalMatrix();

	// StaticMesh ����
	auto staticMeshObject = mGameProcess->mGraphics->CreateStaticMeshObject(meshInfo);
	staticMeshRenderer->SetStaticMeshObject(staticMeshObject);
}

void fq::game_engine::RenderingSystem::loadModel(ModelPath path)
{
	auto iter = mLoadModels.find(path);

	if (iter == mLoadModels.end())
	{
		std::filesystem::path texturePath = path;
		texturePath = texturePath.remove_filename();

		mGameProcess->mGraphics->CreateModel(path, texturePath);
		mLoadModels.insert(path);
	}
}

void fq::game_engine::RenderingSystem::unloadAllModel()
{
	for (auto& modelPath : mLoadModels)
	{
		mGameProcess->mGraphics->DeleteModel(modelPath);
	}

	mLoadModels.clear();
}

void fq::game_engine::RenderingSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	unloadStaticMeshRenderer(event.object);
}

void fq::game_engine::RenderingSystem::unloadStaticMeshRenderer(fq::game_module::GameObject* object)
{
	if (!object->HasComponent<fq::game_module::StaticMeshRenderer>())
	{
		return;
	}

	auto staticMeshRenderer = object->GetComponent<fq::game_module::StaticMeshRenderer>();
	auto staticMesh = staticMeshRenderer->GetStaticMeshObject();
	mGameProcess->mGraphics->DeleteStaticMeshObject(staticMesh);
	staticMeshRenderer->SetStaticMeshObject(nullptr);
}
