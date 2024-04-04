#include "GameModulePCH.h"
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

}

void fq::game_module::Scene::Start()
{

}

void fq::game_module::Scene::FixedUpdate(float dt)
{

}

void fq::game_module::Scene::Update(float dt)
{

}

void fq::game_module::Scene::Finalize()
{

}

void fq::game_module::Scene::LateUpdate(float dt)
{

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

void fq::game_module::Scene::ViewObjects(const std::function<void(const std::shared_ptr<GameObject>&)>& viewFunc
	, bool bIsIncludeToBeDestroyed /*= false*/)
{
	for (const auto& object : GetObjectView(bIsIncludeToBeDestroyed))
	{
		viewFunc(object);
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

