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

		void Initialize(GameProcess* gameProcess);
		void Update(float dt);

		void OnLoadScene();
		void OnCreateStateEvent(const fq::event::OnCreateStateEvent& data);
		void OnDeleteStateEvent(const fq::event::OnDeleteStateEvent& data);

	private:
		struct EventDependObjectKey
		{
			size_t NameKey;
			size_t ObjectKey;

			bool operator<(const EventDependObjectKey& other) const
			{
				if (this->NameKey == other.NameKey)
				{
					return this->ObjectKey < other.ObjectKey;
				}

				return this->NameKey < other.NameKey;
			}
		};

		struct LifeTimeDependObject
		{
			std::shared_ptr<fq::game_module::GameObject> InstantiatePrefabObject;
			float LifeTime;
			float PlaybackSpeed;
		};

		struct EventDependObject
		{
			std::shared_ptr<fq::game_module::GameObject> InstantiatePrefabObject;
		};

		struct PlaySoundLifeTimeInfo
		{
			fq::game_module::GameObject* OnwerGameObject;
			int Channel;
			float LifeTime;
			bool bUseDeleteStateEnd;
		};

	private:
		DirectX::SimpleMath::Vector3 genarateRenderVector(const DirectX::SimpleMath::Vector3& random);
		EventDependObjectKey generateKey(const std::string& keyName, const fq::game_module::GameObject* ownerObject);


	private:
		enum { RESERVE_SIZE = 512 };

		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnProcessStateEvent;
		EventHandler mOnProcessStateExitEvent;

		fq::game_module::StateEvent* mStateEvent;

		std::vector<PlaySoundLifeTimeInfo> mPlaySoundLifeTimeInfos;

		std::vector<LifeTimeDependObject> mLifeTimeDependObjects;
		std::map<EventDependObjectKey, EventDependObject> mEventDependObjects;

		std::random_device mRandomDevice;  // ·£´ý ÀåÄ¡
		std::mt19937 mGenerarator;
	};
}