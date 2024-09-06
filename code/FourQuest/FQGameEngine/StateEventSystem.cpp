#include "StateEventSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Decal.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/GameObject.h"

fq::game_engine::StateEventSystem::StateEventSystem()
	: mGameProcess(nullptr)
	, mRandomDevice()
	, mGenerarator(mRandomDevice())
{
}

fq::game_engine::StateEventSystem::~StateEventSystem()
{
}

void fq::game_engine::StateEventSystem::OnLoadScene()
{
	mInstantiatePrefabMap.clear();
	// 게임오브젝트에 종속적인 이펙트를 저장해두고, 상태 변화가 감지되면 제거하도록 하면 어떨까
	// 상태변화 이벤트는
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	auto stateEventGameObject = scene->GetObjectByName("StateEvent");

	if (stateEventGameObject != nullptr)
	{
		mStateEvent = stateEventGameObject->GetComponent<fq::game_module::StateEvent>();
	}

	if (mStateEvent != nullptr)
	{
		for (const auto& instantiatePrefab : mStateEvent->GetInstantiatePrefabs())
		{
			mInstantiatePrefabMap.insert({ instantiatePrefab.FunctionName, instantiatePrefab });
		}
	}
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
				auto find = mInstantiatePrefabMap.find(data.RegisterKeyName);

				if (find != mInstantiatePrefabMap.end())
				{
					const auto& instantiatePrefab = find->second;

					auto instance = mGameProcess->mPrefabManager->InstantiatePrefabResoure({ instantiatePrefab.PrefabResourceData });
					auto effectObject = *(instance.begin());
					auto effectObjectTransform = effectObject->GetTransform();

					auto firedEventObjectTransform = data.gameObject->GetTransform();

					const auto addtiveTransform = DirectX::SimpleMath::Matrix::CreateScale(instantiatePrefab.Scale) * DirectX::SimpleMath::Matrix::CreateTranslation(instantiatePrefab.Translate + genarateRenderVector(instantiatePrefab.RandomRange));

					if (instantiatePrefab.bIsFollowingParentPosition)
					{
						effectObjectTransform->SetParent(firedEventObjectTransform);
						effectObjectTransform->SetLocalMatrix(addtiveTransform);
					}
					else
					{
						effectObjectTransform->SetWorldMatrix(addtiveTransform * firedEventObjectTransform->GetWorldMatrix());
					}
					

					auto scene = mGameProcess->mSceneManager->GetCurrentScene();
					scene->AddGameObject(effectObject);

					mGameObjectLifeTimes.push_back({ data.gameObject, effectObject, instantiatePrefab.DeleteTime, instantiatePrefab.bUseDeleteByStateEnd });
				}

				mAnimationStateEvents.push_back(data);
			});

	mOnProcessStateExitEvent = eventMgr->
		RegisterHandle<fq::event::AnimationStateExitEvent>(
			[this](const fq::event::AnimationStateExitEvent& data)
			{
				auto scene = mGameProcess->mSceneManager->GetCurrentScene();

				for (auto iter = mGameObjectLifeTimes.begin(); iter != mGameObjectLifeTimes.end();)
				{
					GameObjectLifeTime& gameObjectLifeTime = *iter;

					if (gameObjectLifeTime.bUseDeleteStateEnd && data.gameObject == gameObjectLifeTime.OnwerGameObject)
					{
						scene->DestroyGameObject(gameObjectLifeTime.gameObject.get());
						iter = mGameObjectLifeTimes.erase(iter);

						break;
					}
					else
					{
						++iter;
					}
				}
			});
}

void fq::game_engine::StateEventSystem::Update(float dt)
{
	if (mStateEvent == nullptr)
	{
		return;
	}

	using namespace fq::game_module;
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto iter = mGameObjectLifeTimes.begin(); iter != mGameObjectLifeTimes.end(); )
	{
		GameObjectLifeTime& gameObjectLifeTime = *iter;

		gameObjectLifeTime.LifeTime -= dt;

		if (gameObjectLifeTime.LifeTime < 0)
		{
			scene->DestroyGameObject(gameObjectLifeTime.gameObject.get());
			iter = mGameObjectLifeTimes.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

DirectX::SimpleMath::Vector3 fq::game_engine::StateEventSystem::genarateRenderVector(const DirectX::SimpleMath::Vector3& random)
{
	auto randoemFloat = [this](float min, float max)
		{
			std::uniform_real_distribution<float> dis(min, max);
			return dis(mGenerarator);
		};

	float x = randoemFloat(-random.x, random.x);
	float y = randoemFloat(-random.y, random.y);
	float z = randoemFloat(-random.z, random.z);
	
	return DirectX::SimpleMath::Vector3(x, y, z);
}
