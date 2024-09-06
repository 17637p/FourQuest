#pragma once

#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/StateEvent.h"

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
		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnProcessStateEvent;
		// 이벤트 저장

		std::vector<fq::event::AnimationStateEvent> mAnimationStateEvents;

		struct GameObjectLifeTime
		{
			std::shared_ptr<fq::game_module::GameObject> gameObject;
			float LifeTime;
		};

		std::vector<GameObjectLifeTime> mGameObjectLifeTimes;
	};
}