#pragma once
#include "IEditorWindow.h"

namespace fq::game_engine
{
	class GameProcess;

	class CollisionMatrixWindow : public IEditorWindow
	{
	public:
		CollisionMatrixWindow();
		~CollisionMatrixWindow();

		void Initialize(GameProcess* game);

		void Render()override;

	private:
		GameProcess* mGameProcess;

	};


}