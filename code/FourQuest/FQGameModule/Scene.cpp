#include "Scene.h"

#include <queue>
#include <spdlog/spdlog.h>

#include "GameObject.h"
#include "SceneHelper.h"
#include "InputManager.h"
#include "EventManager.h"
#include "Event.h"
#include "Transform.h"
#include "ScreenManager.h"

fq::game_module::Scene::Scene()
	:mObjects{}
	, mSceneName{}
	, mInputManager(nullptr)
	, mEventManager(nullptr)
	, mPrefabManager(nullptr)
	, mScreenManager(nullptr)
	, mIsStartScene(false)
	, mPedingObjects{}
{}

fq::game_module::Scene::~Scene()
{
	mObjects.clear();
}

void fq::game_module::Scene::Initialize(std::string sceneName
	, EventManager* eventMgr
	, InputManager* inputMgr
	, PrefabManager* prefabMgr
	, ScreenManager* screenMgr
	, TimeManager* timeMgr
	, SoundManager* soundMgr)
{
	mSceneName = std::move(sceneName);
	mEventManager = eventMgr;
	mInputManager = inputMgr;
	mPrefabManager = prefabMgr;
	mScreenManager = screenMgr;
	mTimeManager = timeMgr;
	mSoundManager = soundMgr;
}


fq::game_module::GameObject* fq::game_module::Scene::GetObjectByIndex(size_t index)
{
	if (mObjects.size() <= index)
	{
		return nullptr;
	}

	return mObjects[index].get();
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

void fq::game_module::Scene::CleanUp(bool isRemoveComponents)
{
	mEventManager->FireEvent<fq::event::OnCleanUp>({});

	// 삭제 예정인 오브젝트 제거합니다
	mObjects.erase(std::remove_if(mObjects.begin(), mObjects.end()
		, [isRemoveComponents](const std::shared_ptr<GameObject>& object)
		{
			if (object->IsDestroyed() && isRemoveComponents)
			{
				//object->RemoveAllComponent();
			}

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

	if (mIsStartScene) // 씬이 시작된경우 프레임 마지막에 추가합니다
	{
		auto children = object->GetChildren();

		for (auto child : children)
		{
			AddGameObject(child->shared_from_this());
		}

		mPedingObjects.push_back(object);
	}
	else // 씬이 시작하지 않은경우 즉시 추가합니다
	{
		mObjects.push_back(object);

		for (auto child : object->GetChildren())
		{
			AddGameObject(child->shared_from_this());
		}

		mEventManager->FireEvent<fq::event::AddGameObject>({ object.get() });
	}
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

	// 한 프레임에 생성과 삭제를 동시에하는 경우 예외처리
	bool hasPedingObject = false;
	mPedingObjects.erase(std::remove_if(mPedingObjects.begin(), mPedingObjects.end()
		, [&hasPedingObject, object](const std::shared_ptr<GameObject>& pedingObject)
		{
			if (pedingObject->GetID() == object->GetID())
			{
				hasPedingObject = true;
				return true;
			}

			return false;
		}), mPedingObjects.end());

	if (!hasPedingObject)
	{
		object->OnDestroy();
		mEventManager->FireEvent<fq::event::OnDestoryedGameObject>({ object });
	}
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

	// 한 프레임에 생성과 삭제를 동시에하는 경우 예외처리
	bool hasPedingObject = false;
	mPedingObjects.erase(std::remove_if(mPedingObjects.begin(), mPedingObjects.end()
		, [&hasPedingObject, object](const std::shared_ptr<GameObject>& pedingObject)
		{
			if (pedingObject->GetID() == object->GetID())
			{
				hasPedingObject = true;
				return true;
			}

			return false;
		}), mPedingObjects.end());

	if (!hasPedingObject)
	{
		object->OnDestroy();
		mEventManager->FireEvent<fq::event::OnDestoryedGameObject>({ object });
	}
}
void fq::game_module::Scene::DestroyAll()
{
	for (auto& object : mObjects)
	{
		object->mbIsDestroyed = true;
		object->OnDestroy();
		mEventManager->FireEvent<fq::event::OnDestoryedGameObject>({ object.get() });
	}

	mPedingObjects.clear();
	mObjects.clear();
}

std::shared_ptr<fq::game_module::GameObject> fq::game_module::Scene::GetObjectByName(std::string name)
{
	for (const auto& object : mObjects)
	{
		if (object->GetName() == name)
		{
			return object;
		}

	}

	return  nullptr;
}

void fq::game_module::Scene::processPedingObject()
{
	for (int i = 0; i < mPedingObjects.size(); ++i)
	{
		auto object = mPedingObjects[i];
		mEventManager->FireEvent<fq::event::AddGameObject>({ object.get() });
		object->OnAwake();
		object->OnStart();
		mObjects.push_back(std::move(object));
	}

	mPedingObjects.clear();
}

