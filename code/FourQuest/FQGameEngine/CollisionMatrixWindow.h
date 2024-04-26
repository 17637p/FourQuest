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

	private:
		void beginButton_Reset();
		void beginCollisionMatrix();

	private:
		bool mbIsOpen;

		GameProcess* mGameProcess;
		PhysicsSystem* mPhysicsSystem;
	};


}