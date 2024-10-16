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

		/// <summary>
		/// �ε��� �ߴܵ� ��� true �� ��ȯ�մϴ�.
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