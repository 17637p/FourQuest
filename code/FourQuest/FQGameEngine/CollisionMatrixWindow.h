#pragma once
#include "IEditorWindow.h"

namespace fq::game_engine
{
	class GameProcess;
	class PhysicsSystem;

	class CollisionMatrixWindow : public IEditorWindow
	{
	public:
		CollisionMatrixWindow();
		~CollisionMatrixWindow();

		void Initialize(GameProcess* game);
		void Render()override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void beginButton_Reset();
		void beginCollisionMatrix();
		void beginButton_Save();

	private:
		bool mbIsOpen;

		GameProcess* mGameProcess;
		PhysicsSystem* mPhysicsSystem;
	};


}