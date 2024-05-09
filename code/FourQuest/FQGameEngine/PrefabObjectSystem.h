#pragma once

#include <unordered_map>
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// ������ ������Ʈ�� �����ϴ� �ý��� 
	/// </summary>
	class PrefabObjectSystem
	{
		using EventHandler = fq::game_module::EventHandler;

	public:
		PrefabObjectSystem();
		~PrefabObjectSystem();

		void Initialize(GameProcess* game);

		void OnLoadScene(const fq::event::OnLoadScene& event);

		void OnUnloadScene();

		void InstantiatePrefab(fq::event::InstantiatePrefab event);
	private:
		fq::game_module::ObjectManager* mObjectManager;
		fq::game_module::EventManager* mEventManager;

		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
	};
}

