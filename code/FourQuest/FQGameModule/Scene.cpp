#include "Scene.h"
#include "GameObject.h"

fq::game_module::Scene::Scene()
	:mLastObjectID(0)
	,mObjects{}
{}

fq::game_module::Scene::~Scene()
{}

void fq::game_module::Scene::Initialize()
{
	for (const auto& object : mObjects)
	{
		object->Initialize();
	}
}

void fq::game_module::Scene::Start()
{
	for (const auto& object : mObjects)
	{
		object->Start();
	}
}

void fq::game_module::Scene::FixedUpdate(float dt)
{
	for (const auto& object : mObjects)
	{
		object->FixedUpdate(dt);
	}
}

void fq::game_module::Scene::Update(float dt)
{
	for (const auto& object : mObjects)
	{
		object->Update(dt);
	}
}

void fq::game_module::Scene::LateUpdate(float dt)
{
	for (const auto& object : mObjects)
	{
		object->LateUpdate(dt);
	}
}

void fq::game_module::Scene::Finalize()
{
	for (const auto& object : mObjects)
	{
		object->Finalize();
	}
}


std::shared_ptr<fq::game_module::GameObject> fq::game_module::Scene::GetObjectByIndex(size_t index)
{
	if (mObjects.size() <= index)
	{
		return nullptr;
	}

	return mObjects[index];
}

fq::game_module::internal::GameObjectView fq::game_module::Scene::GetObjectView(bool bIsIncludeToBeDestroyed /*= false*/)
{
	internal::GameObjectIterator begin(this, 0, false, bIsIncludeToBeDestroyed);

	internal::GameObjectIterator end(this, 0, true, bIsIncludeToBeDestroyed);

	return internal::GameObjectView(begin, end);
}

void fq::game_module::Scene::ViewObjects(std::function<void(GameObject&)> viewFunction
	, bool bIsIncludeToBeDestroyed /*= false*/)
{
	for (GameObject& object : GetObjectView(bIsIncludeToBeDestroyed))
	{
		viewFunction(object);
	}
}

std::shared_ptr<fq::game_module::GameObject> fq::game_module::Scene::GetObjectByID(unsigned int id)
{
	for (const auto& object : mObjects)
	{
		if (id == object->GetID())
		{
			return object;
		}
	}

	return nullptr;
}

void fq::game_module::Scene::CleanUp()
{
	mObjects.erase(std::remove_if(mObjects.begin(), mObjects.end()
		, [](const std::shared_ptr<GameObject>& object)
		{
			return object->IsToBeDestroyed();
		}), mObjects.end());
}

