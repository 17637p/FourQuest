#pragma once
#pragma once

#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/GameModule.h"

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
		void updateUI();
		void setRenderUI(bool isRender);

	private:
		GameProcess* mGameProcess;
		fq::graphics::IImageObject* mLoadImage;
		std::vector<std::shared_ptr<fq::game_module::GameObject>> mLoadingUIObject;
	};
}