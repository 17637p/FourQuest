#pragma once

#include "../FQGraphics/IFQGraphics.h"

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// �ε� ������ Ŭ���� 
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