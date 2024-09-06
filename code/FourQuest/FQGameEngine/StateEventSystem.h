#pragma once

#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/StateEvent.h"
#include <random>

namespace fq::game_module
{
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;

	class StateEventSystem
	{
		using EventHandler = fq::game_module::EventHandler;

	public:
		StateEventSystem();
		~StateEventSystem();

		void OnLoadScene();

		void Initialize(GameProcess* gameProcess);
		void Update(float dt);

	private:
		DirectX::SimpleMath::Vector3 genarateRenderVector(const DirectX::SimpleMath::Vector3& random);

	private:
		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnProcessStateEvent;
		EventHandler mOnProcessStateExitEvent;
		// 이벤트 저장

		std::vector<fq::event::AnimationStateEvent> mAnimationStateEvents;

		struct GameObjectLifeTime
		{
			fq::game_module::GameObject* OnwerGameObject;
			std::shared_ptr<fq::game_module::GameObject> gameObject;
			float LifeTime;
			bool bUseDeleteStateEnd;
		};

		std::vector<GameObjectLifeTime> mGameObjectLifeTimes;

		fq::game_module::StateEvent* mStateEvent;

		std::map<std::string, fq::game_module::StateEvent::InstantiatePrefab> mInstantiatePrefabMap;

		std::random_device mRandomDevice;  // 랜덤 장치
		std::mt19937 mGenerarator;
	};
}