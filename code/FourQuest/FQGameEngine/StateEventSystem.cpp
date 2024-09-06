#include "StateEventSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Decal.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/GameObject.h"

fq::game_engine::StateEventSystem::StateEventSystem()
	: mGameProcess(nullptr)
{
}

fq::game_engine::StateEventSystem::~StateEventSystem()
{
}

void fq::game_engine::StateEventSystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<fq::game_module::StateEvent>(
		[this](fq::game_module::GameObject& object, fq::game_module::StateEvent& stateEvent)
		{
			const auto& instantiatePrefabs = stateEvent.GetInstantiatePrefabs();

			for (const auto& instantiatePrefab : instantiatePrefabs)
			{
				auto instance = mGameProcess->mPrefabManager->LoadPrefab(instantiatePrefab.PrefabResource);
			}
		}
	);

	// 게임오브젝트에 종속적인 이펙트를 저장해두고, 상태 변화가 감지되면 제거하도록 하면 어떨까
	// 상태변화 이벤트는
}

void fq::game_engine::StateEventSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle 등록
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &StateEventSystem::OnLoadScene);

	mOnProcessStateEvent = eventMgr->
		RegisterHandle<fq::event::AnimationStateEvent>(
			[this](const fq::event::AnimationStateEvent& data)
			{
				mAnimationStateEvents.push_back(data);
			});

	//stateEvent 취득
}

void fq::game_engine::StateEventSystem::Update(float dt)
{
	using namespace fq::game_module;
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<fq::game_module::StateEvent>(
		[this, scene](fq::game_module::GameObject& object, fq::game_module::StateEvent& stateEvent)
		{
			const auto& instantiatePrefabs = stateEvent.GetInstantiatePrefabs();

			for (const auto& instantiatePrefab : instantiatePrefabs)
			{
				for (const auto& event : mAnimationStateEvents)
				{
					if (instantiatePrefab.FunctionName == event.RegisterKeyName)
					{
						auto instance = mGameProcess->mPrefabManager->InstantiatePrefabResoure({ PrefabResource{instantiatePrefab.PrefabResource} });
						auto& effectObject = *(instance.begin());
						auto effectObjectTransform = effectObject->GetTransform();

						auto firedEventObjectTransform = event.gameObject->GetTransform();

						effectObjectTransform->SetParent(firedEventObjectTransform);

						scene->AddGameObject(effectObject);

						mGameObjectLifeTimes.push_back({ effectObject, instantiatePrefab.DeleteTime });
					}
				}
			}
		}
	);

	mAnimationStateEvents.clear();

	for (auto iter = mGameObjectLifeTimes.begin(); iter != mGameObjectLifeTimes.end(); )
	{
		GameObjectLifeTime& gameObjectLifeTime = *iter;

		gameObjectLifeTime.LifeTime -= dt;

		if (gameObjectLifeTime.LifeTime < 0)
		{
			gameObjectLifeTime.gameObject->OnDestroy();
			iter = mGameObjectLifeTimes.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}
