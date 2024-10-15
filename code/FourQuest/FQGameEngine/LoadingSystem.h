#pragma once
#pragma once

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// 로딩 쓰레드 클래스 
	/// </summary>
	class LoadingSystem
	{
	public:
		LoadingSystem();
		~LoadingSystem();

		void Initialize(GameProcess* game);
		void Finalize();

		/// <summary>
		/// 로딩이 중단된 경우 true 를 반환합니다.
		/// </summary>
		/// <returns></returns>
		bool ProcessLoading();

	private:
		void loadUI();
		void updateUI(float dt);
		void setRenderUI(bool isRender);

	private:
		GameProcess* mGameProcess;
		std::vector<std::shared_ptr<fq::game_module::GameObject>> mLoadingUIObject;
	};
}