#pragma once
#include "IEditorWindow.h"

namespace fq::game_engine
{
	class GameProcess;
	class NavigationMeshBuilder;

	class NavigationMeshWindow : public IEditorWindow
	{
	public:
		NavigationMeshWindow();
		virtual ~NavigationMeshWindow();

		virtual void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void beginButtonBuild();
		void beginNavigationMeshDataInput();

	private:
		bool mbIsOpen;

		GameProcess* mGameProcess;
		NavigationMeshBuilder* mNavigationMeshBuilder;
	};
}

