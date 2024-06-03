#pragma once
#include "IEditorWindow.h"

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

	private:
		void beginChild_SnapSetting();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		bool mbUseSnap;
		float mSnap[3];

		bool mbIsOpen;
	};


}