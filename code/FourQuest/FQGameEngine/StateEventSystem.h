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
		void OnAnimationStateEvent(const fq::event::AnimationStateEvent& data);

		void Initialize(GameProcess* gameProcess);
		void Update(float dt);

	private:
		DirectX::SimpleMath::Vector3 genarateRenderVector(const DirectX::SimpleMath::Vector3& random);

	private:
		struct InstantiatePrefabLifeTimeInfo
		{
			fq::game_module::GameObject* OnwerGameObject;
			std::shared_ptr<fq::game_module::GameObject> InstantiatePrefabObject;
			float LifeTime;
			bool bUseDeleteStateEnd;
			float PlaybackSpeed;
		};

		struct PlaySoundLifeTimeInfo
		{
			fq::game_module::GameObject* OnwerGameObject;
			int Channel;
			float LifeTime;
			bool bUseDeleteStateEnd;
		};

	private:
		enum { RESERVE_SIZE = 512 };

		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnProcessStateEvent;
		EventHandler mOnProcessStateExitEvent;

		fq::game_module::StateEvent* mStateEvent;

		std::vector<InstantiatePrefabLifeTimeInfo> mInstantiatePrefabLifeTimeInfos;
		std::vector<PlaySoundLifeTimeInfo> mPlaySoundLifeTimeInfos;

		std::random_device mRandomDevice;  // ���� ��ġ
		std::mt19937 mGenerarator;
	};
}