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

		void OnPlaySound(fq::event::OnPlaySound event);

		void OnStopChannel(fq::event::OnStopChannel event);
	private:
		fq::game_module::Scene* mScene;
		fq::game_module::SoundManager* mSoundManager;
		fq::game_module::EventManager* mEventManager;

		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnPlaySoundHandler;
		EventHandler mOnStopChannelHandler;

		bool mbIsGameLoaded;
	};
}

