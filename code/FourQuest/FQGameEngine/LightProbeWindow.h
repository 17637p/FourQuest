#pragma once
#include "IEditorWindow.h"

#include <string>

namespace fq::game_module
{
	class InputManager;
}

namespace fq::graphics
{
	class IProbeObject;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;
	class LightProbeSystem;

	class LightProbeWindow : public IEditorWindow
	{
	public:
		LightProbeWindow();
		virtual ~LightProbeWindow();

		void Initialize(GameProcess* game, EditorProcess* editor);
		virtual void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }
		void PickObject(void* iProbeObject); // GamePlayWindow 에서 호출 

	private:
		void beginButtons();
		void beginProbeList();
		void CheckCopyPaste();
		void CancelSelect();
		// add
		// delete
		// save
		// load

	private:
		bool mbIsOpen;
		std::string mSaveFileName;

		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		LightProbeSystem* mLightProbeSystem;

		fq::game_module::InputManager* mInputManager;
		fq::graphics::IProbeObject* mSelectObject;

		int mWidth;
		int mHeight;
		float mScale;
		float mIntensity;
		bool mIsDrawDebugLightProbe;

		// Copy and Paste
		fq::graphics::IProbeObject* mCopyObject;
	};
}

