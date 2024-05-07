#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;

	class SoundSystem
	{
		using EventHandler = fq::game_module::EventHandler;
	public:

		void Initialize(fq::game_engine::GameProcess* game);

		/// <summary>
		/// ���� �ε��Ҷ� �������� ���õ� ���ҽ��� �ε��մϴ�
		/// </summary>
		void OnLoadScene(const fq::event::OnLoadScene event);

		/// <summary>
		/// ���� ��ε��Ҷ� �������� ���õ� ���ҽ��� ��ε��մϴ�.
		/// </summary>
		void OnUnLoadScene();

		/// <summary>
		/// ���ӿ�����Ʈ �߰� �̺�Ʈ
		/// </summary>
		void OnAddGameObject(const fq::event::AddGameObject& event);

		/// <summary>
		/// AddComponent �̺�Ʈ ó��
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);


	private:
		fq::game_module::Scene* mScene;
		fq::game_module::SoundManager* mSoundManager;
		fq::game_module::EventManager* mEventManager;

		EventHandler mAddComponentHandler;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;

		bool mbIsGameLoaded;
	};
}

