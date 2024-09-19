#pragma once
#include "IEditorWindow.h"

#include <string>

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
		std::string mSaveFileName;
		std::string mObjectName;

		GameProcess* mGameProcess;
		PathFindingSystem* mPathFindingSystem;
	};
}

