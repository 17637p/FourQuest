#pragma once

#include "IEditorWindow.h"

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

#include <memory>

namespace fq::game_module
{
	class ArticulationData;
	class LinkData;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;
	class Articulation;
	
	class ArticulationInspector : public IEditorWindow
	{
	public:
		ArticulationInspector();
		~ArticulationInspector();

		void Initialize(GameProcess* game, EditorProcess* editor);

		virtual void Render() override;

		inline bool& IsWindowOpen();

	private:
		void beginLinkData();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		bool mbIsOpen;

		std::shared_ptr<fq::game_module::ArticulationData> mArticulationData;
		std::shared_ptr<fq::game_module::LinkData> mSelectLink;

		fq::game_module::EventHandler mSelectObjectHandler;
	};

	bool& ArticulationInspector::IsWindowOpen()
	{
		return mbIsOpen;
	}
}
