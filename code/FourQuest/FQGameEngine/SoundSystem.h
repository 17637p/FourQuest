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
		/// 씬을 로드할때 랜더링에 관련된 리소스를 로드합니다
		/// </summary>
		void OnLoadScene(const fq::event::OnLoadScene event);

		/// <summary>
		/// 씬을 언로드할때 랜더링에 관련된 리소스를 언로드합니다.
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

