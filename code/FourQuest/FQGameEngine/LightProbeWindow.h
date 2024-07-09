#pragma once
#include "IEditorWindow.h"

#include <string>

namespace fq::game_engine
{
	class GameProcess;
	class LightProbeSystem;

	class LightProbeWindow : public IEditorWindow
	{
	public:
		LightProbeWindow();
		virtual ~LightProbeWindow();

		void Initialize(GameProcess* game);
		virtual void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

		void beginButtons();
		// add
		// delete
		// save
		// load

	private:

	private:
		bool mbIsOpen;
		std::string mSaveFileName;

		GameProcess* mGameProcess;
		LightProbeSystem* mLightProbeSystem;
	};
}

