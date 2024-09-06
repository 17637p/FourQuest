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

	// ���ӿ�����Ʈ�� �������� ����Ʈ�� �����صΰ�, ���� ��ȭ�� �����Ǹ� �����ϵ��� �ϸ� ���
	// ���º�ȭ �̺�Ʈ��
}

void fq::game_engine::StateEventSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle ���
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &StateEventSystem::OnLoadScene);

	mOnProcessStateEvent = eventMgr->
		RegisterHandle<fq::event::AnimationStateEvent>(
			[this](const fq::event::AnimationStateEvent& data)
			{
				mAnimationStateEvents.push_back(data);
			});

	//stateEvent ���
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
