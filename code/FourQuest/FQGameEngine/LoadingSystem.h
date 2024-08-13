#pragma once

#include "../FQGraphics/IFQGraphics.h"

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
		void ProcessLoading();
	private:
		void loadUI();

	private:
		GameProcess* mGameProcess;
		fq::graphics::IImageObject* mLoadImage;
	};
}