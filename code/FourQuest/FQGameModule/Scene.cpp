#include "Scene.h"
#include "GameObject.h"
#include "SceneHelper.h"
#include "InputManager.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::Scene::Scene()
	:mObjects{}
	,mSceneName{}
	,mInputManager(nullptr)
	,mEventManager(nullptr)
{}

fq::game_module::Scene::~Scene()
{}

void fq::game_module::Scene::Initialize(std::string sceneName, EventManager* eventMgr, InputManager* inputMgr)
{
	mSceneName = std::move(sceneName);
	mEventManager = eventMgr;
	mInputManager = inputMgr;

	// load

	// awake
	for (const auto& object : mObjects)
	{
		object->OnAwake();
	}
}

void fq::game_module::Scene::Start()
{
	for (const auto& object : mObjects)
	{
		object->OnStart();
	}
}

void fq::game_module::Scene::FixedUpdate(float dt)
{
	for (const auto& object : mObjects)
	{
		object->OnFixedUpdate(dt);
	}
}

void fq::game_module::Scene::Update(float dt)
{
	for (const auto& object : mObjects)
	{
		object->OnUpdate(dt);
	}
}

void fq::game_module::Scene::LateUpdate(float dt)
{
	for (const auto& object : mObjects)
	{
		object->OnLateUpdate(dt);
	}
}

void fq::game_module::Scene::Finalize()
{
	for (const auto& object : mObjects)
	{
		object->OnDestroy();
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

	internal::GameObjectIterator end(this, GetObjectSize(), true, bIsIncludeToBeDestroyed);

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

void fq::game_module::Scene::AddGameObject(std::shared_ptr<GameObject> object)
{
	for (const auto& inObject : mObjects)
	{
		if (inObject.get() == object.get())
		{
			assert(nullptr);
		}
	}

	object->SetScene(this);
	SceneHeleper::CheckNameDuplication(*this, *object);
	object->mbIsToBeDestroyed = false;
	
	mObjects.push_back(std::move(object));

	mEventManager->FireEvent<fq::event::AddGameObject>({ object.get() });
}

void fq::game_module::Scene::DestroyGameObject(GameObject* object)
{
	if (object->IsToBeDestroyed())
	{
		return;
	}

	object->OnDestroy();

	mEventManager->FireEvent<fq::event::OnGameObjectDestroyed>({ object });
}


