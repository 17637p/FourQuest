#pragma once

#include "../FQGameModule/EventHandler.h"

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// 랜더링관련 바인딩 처리를 담당합니다
	/// </summary>
	class RenderingSystem
	{
		using EventHandler = fq::game_module::EventHandler;
	public:
		RenderingSystem();
		~RenderingSystem();

		/// <summary>
		/// 랜더링 시스템을 초기화합니다
		/// </summary>
		void Initialize(GameProcess* gameProcess);

		void Update(float dt);

		/// <summary>
		/// 씬을 로드할때 랜더링에 관련된 리소스를 로드합니다
		/// </summary>
		void LoadScene();

		/// <summary>
		/// 씬을 언로드할때 랜더링에 관련된 리소스를 언로드합니다.
		/// </summary>
		void UnLoadScene();

	private:
		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
	};
}