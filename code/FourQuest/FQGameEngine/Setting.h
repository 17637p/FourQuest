#pragma once
#include "IEditorWindow.h"

#include <imgui.h>
#include <ImGuizmo.h>

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class Setting : public IEditorWindow
	{
	public:
		Setting();

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

		bool UseSnap() const { return mbUseSnap; }

		float* GetSnap() { return &mSnap[0]; }

		ImGuizmo::MODE GetMode();

	private:
		void beginChild_GizumoSetting();
		void beginChild_GraphicsSetting();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		bool mbUseSnap;
		float mSnap[3];
		ImGuizmo::MODE mMode;

		bool mbIsOpen;
		bool mbUseGrayScale;
	};


}