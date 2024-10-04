#include "StateEventSystem.h"

#include "GameProcess.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Decal.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/GameObject.h"
#include "../FQGameModule/SoundManager.h"
#include "../FQGameModule/UVAnimator.h"
#include "../FQGameModule/Animator.h"

fq::game_engine::StateEventSystem::StateEventSystem()
	: mGameProcess(nullptr)
	, mStateEvent(nullptr)
	, mLifeTimeDependObjects{}
	, mPlaySoundLifeTimeInfos{}
	, mRandomDevice()
	, mGenerarator(mRandomDevice())
{
	mLifeTimeDependObjects.reserve(RESERVE_SIZE);
	mPlaySoundLifeTimeInfos.reserve(RESERVE_SIZE);
}

fq::game_engine::StateEventSystem::~StateEventSystem()
{
}

void fq::game_engine::StateEventSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// EventHandle 등록
	auto& eventMgr = mGameProcess->mEventManager;

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &StateEventSystem::OnLoadScene);

	mOnProcessStateEvent = eventMgr->
		RegisterHandle<fq::event::OnCreateStateEvent>(this, &StateEventSystem::OnCreateStateEvent);

	mOnProcessStateExitEvent = eventMgr->
		RegisterHandle<fq::event::OnDeleteStateEvent>(this, &StateEventSystem::OnDeleteStateEvent);
}

void fq::game_engine::StateEventSystem::Update(float dt)
{
	using namespace fq::game_module;
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for (auto iter = mLifeTimeDependObjects.begin(); iter != mLifeTimeDependObjects.end();)
	{
		LifeTimeDependObject& InstantiatePrefabLifeTimeInfo = *iter;

		InstantiatePrefabLifeTimeInfo.LifeTime -= dt * InstantiatePrefabLifeTimeInfo.PlaybackSpeed;

		if (InstantiatePrefabLifeTimeInfo.LifeTime < 0
			|| InstantiatePrefabLifeTimeInfo.InstantiatePrefabObject->IsDestroyed())
		{
			scene->DestroyGameObject(InstantiatePrefabLifeTimeInfo.InstantiatePrefabObject.get());
			iter = mLifeTimeDependObjects.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	for (auto iter = mEventDependObjects.begin(); iter != mEventDependObjects.end();)
	{
		EventDependObject& InstantiatePrefabLifeTimeInfo = iter->second;

		if (InstantiatePrefabLifeTimeInfo.InstantiatePrefabObject->IsDestroyed())
		{
			iter = mEventDependObjects.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	for (auto iter = mPlaySoundLifeTimeInfos.begin(); iter != mPlaySoundLifeTimeInfos.end();)
	{
		PlaySoundLifeTimeInfo& playSoundLifeTimeInfo = *iter;

		playSoundLifeTimeInfo.LifeTime -= dt;

		if (playSoundLifeTimeInfo.LifeTime < 0)
		{
			mGameProcess->mSoundManager->StopChannel(playSoundLifeTimeInfo.Channel);
			iter = mPlaySoundLifeTimeInfos.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void fq::game_engine::StateEventSystem::OnLoadScene()
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	auto stateEventGameObject = scene->GetObjectByName("StateEvent");

	if (stateEventGameObject != nullptr)
	{
		mStateEvent = stateEventGameObject->GetComponent<fq::game_module::StateEvent>();
	}

	scene->ViewComponents<fq::game_module::StateEvent>(
		[this](fq::game_module::GameObject& object, fq::game_module::StateEvent& stateEvent)
		{
			stateEvent.UpdateMapData();

			for (const auto& [key, data] : stateEvent.GetPlaySoundInfoMap())
			{
				mGameProcess->mSoundManager->LoadSound(key, data.SoundPath);
			}
		}
	);
}

void fq::game_engine::StateEventSystem::OnDeleteStateEvent(const fq::event::OnDeleteStateEvent& data)
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	const auto key = generateKey(data.RegisterKeyName, data.gameObject);
	auto find = mEventDependObjects.find(key);

	if (find != mEventDependObjects.end())
	{
		scene->DestroyGameObject(find->second.InstantiatePrefabObject.get());
		mEventDependObjects.erase(find);
	}

	for (auto iter = mPlaySoundLifeTimeInfos.begin(); iter != mPlaySoundLifeTimeInfos.end();)
	{
		const PlaySoundLifeTimeInfo& playSoundLifeTimeInfo = *iter;

		if (playSoundLifeTimeInfo.bUseDeleteStateEnd)
		{
			mGameProcess->mSoundManager->StopChannel(playSoundLifeTimeInfo.Channel);
			iter = mPlaySoundLifeTimeInfos.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

fq::game_engine::StateEventSystem::EventDependObjectKey fq::game_engine::StateEventSystem::generateKey(const std::string& keyName, const fq::game_module::GameObject* ownerObject)
{
	using namespace fq::game_engine;

	StateEventSystem::EventDependObjectKey key;

	key.NameKey = std::hash<std::string>{}(keyName);
	key.ObjectKey = reinterpret_cast<size_t>(ownerObject);

	return key;
}

void fq::game_engine::StateEventSystem::OnCreateStateEvent(const fq::event::OnCreateStateEvent& data)
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	scene->ViewComponents<fq::game_module::StateEvent>(
		[this, &data](fq::game_module::GameObject& object, fq::game_module::StateEvent& stateEvent)
		{
			// 프리팹 생성 이벤트 처리
			{
				const auto& instantiatePrefabMap = stateEvent.GetInstantiatePrefabMap();

				auto find = instantiatePrefabMap.find(data.RegisterKeyName);

				if (find != instantiatePrefabMap.end())
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

					// 아마 자식 계층까지 파고 들어가야 할거 같음
					if (!instantiatePrefab.bIsPlaybackSppedHandleChildHierarchy)
					{
						auto uvAnimator = effectObject->GetComponent<fq::game_module::UVAnimator>();
						if (uvAnimator != nullptr)
						{
							uvAnimator->SetPlaySpeed(instantiatePrefab.PlayBackSpeed);
						}

						auto animator = effectObject->GetComponent<fq::game_module::Animator>();
						if (animator != nullptr)
						{
							animator->SetPlaySpeed(instantiatePrefab.PlayBackSpeed);
						}
					}
					else
					{
						std::function<void(fq::game_module::GameObject*, float)> setPlayBackSpeedRecursive = [&setPlayBackSpeedRecursive](fq::game_module::GameObject* gameObject, float playBackSpeed)
							{
								auto uvAnimator = gameObject->GetComponent<fq::game_module::UVAnimator>();
								if (uvAnimator != nullptr)
								{
									uvAnimator->SetPlaySpeed(playBackSpeed);
								}

								auto animator = gameObject->GetComponent<fq::game_module::Animator>();
								if (animator != nullptr)
								{
									animator->SetPlaySpeed(playBackSpeed);
								}

								for (auto child : gameObject->GetChildren())
								{
									setPlayBackSpeedRecursive(child, playBackSpeed);
								}
							};

						setPlayBackSpeedRecursive(effectObject.get(), instantiatePrefab.PlayBackSpeed);
					}

					// 자료구조 추가
					if (instantiatePrefab.bUseAutoDelete)
					{
						LifeTimeDependObject objectData;
						objectData.InstantiatePrefabObject = effectObject;
						objectData.LifeTime = instantiatePrefab.DeleteTime;
						objectData.PlaybackSpeed = instantiatePrefab.PlayBackSpeed;

						mLifeTimeDependObjects.push_back(objectData);
					}
					if (instantiatePrefab.bUseDeleteByStateEnd)
					{
						EventDependObject objectData;
						objectData.InstantiatePrefabObject = effectObject;

						mEventDependObjects.insert({ generateKey(instantiatePrefab.FunctionName, data.gameObject), objectData });
					}

					if (instantiatePrefab.bIsGenerateEvent)
					{
						fq::event::OnGeneratePrefabInstance generateData;
						generateData.EventSendObject = data.gameObject;
						generateData.PrefabGameObject = effectObject;

						mGameProcess->mEventManager->FireEvent<fq::event::OnGeneratePrefabInstance>(std::move(generateData));
					}
				}
			}

			// 사운드 이벤트 처리
			{
				const auto playerSoundInfoMap = stateEvent.GetPlaySoundInfoMap();

				auto find = playerSoundInfoMap.find(data.RegisterKeyName);

				if (find != playerSoundInfoMap.end())
				{
					const auto& playerSoundInfo = find->second;

					if (playerSoundInfo.bUseAutoDelete)
					{
						PlaySoundLifeTimeInfo playSoundLifeTimeInfo;
						playSoundLifeTimeInfo.OnwerGameObject = data.gameObject;
						playSoundLifeTimeInfo.Channel = playerSoundInfo.Channel;
						playSoundLifeTimeInfo.LifeTime = playerSoundInfo.DeleteTime;
						playSoundLifeTimeInfo.bUseDeleteStateEnd = playerSoundInfo.bUseDeleteByStateEnd;

						mPlaySoundLifeTimeInfos.push_back(playSoundLifeTimeInfo);
					}

					mGameProcess->mSoundManager->Play(playerSoundInfo.FunctionName, playerSoundInfo.bIsLoop, playerSoundInfo.Channel);
				}
			}
		}
	);
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
