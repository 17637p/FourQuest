#pragma once
#include "IEditorWindow.h"

namespace fq::game_engine
{
	class GameProcess;
	class PathFindingSystem;

	class NavigationMeshWindow : public IEditorWindow
	{
	public:
		NavigationMeshWindow();
		virtual ~NavigationMeshWindow();

		void Initialize(GameProcess* game);
		virtual void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void beginButtonBuild();
		void beginNavigationMeshDataInput();

	private:
		bool mbIsOpen;

		GameProcess* mGameProcess;
		PathFindingSystem* mPathFindingSystem;
	};
}

