#include "Scene.h"

#include <queue>

#include "GameObject.h"
#include "SceneHelper.h"
#include "InputManager.h"
#include "EventManager.h"
#include "Event.h"
#include "Transform.h"

fq::game_module::Scene::Scene()
	:mObjects{}
	, mSceneName{}
	, mInputManager(nullptr)
	, mEventManager(nullptr)
{}

fq::game_module::Scene::~Scene()
{}

void fq::game_module::Scene::Initialize(std::string sceneName, EventManager* eventMgr, InputManager* inputMgr)
{
	mSceneName = std::move(sceneName);
	mEventManager = eventMgr;
	mInputManager = inputMgr;
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
	// 삭제 예정인 오브젝트 제거합니다
	mObjects.erase(std::remove_if(mObjects.begin(), mObjects.end()
		, [](const std::shared_ptr<GameObject>& object)
		{
			return object->IsDestroyed();
		}), mObjects.end());

	// 삭제 예정인 컴포넌트를 제거합니다
	for (const auto& object : mObjects)
	{
		object->CleanUpComponent();
	}
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
	object->mbIsDestroyed = false;

	mObjects.push_back(object);

	for (auto child : object->GetChildren())
	{
		AddGameObject(child->shared_from_this());
	}

	mEventManager->FireEvent<fq::event::AddGameObject>({ object.get() });
}

void fq::game_module::Scene::DestroyGameObject(GameObject* object)
{
	if (object->IsDestroyed())
	{
		return;
	}

	object->mbIsDestroyed = true;

	// 계층구조 연결을 해제합니다 
	object->GetComponent<fq::game_module::Transform>()->SetParent(nullptr);

	for (auto child : object->GetChildren())
	{
		destroyChild(child);
	}

	object->OnDestroy();
	mEventManager->FireEvent<fq::event::OnGameObjectDestroyed>({ object });
}

void fq::game_module::Scene::destroyChild(GameObject* object)
{
	if (object->IsDestroyed())
	{
		return;
	}

	object->mbIsDestroyed = true;

	for (auto child : object->GetChildren())
	{
		destroyChild(child);
	}

	object->OnDestroy();
	mEventManager->FireEvent<fq::event::OnGameObjectDestroyed>({ object });
}

void fq::game_module::Scene::DestroyAll()
{

}

