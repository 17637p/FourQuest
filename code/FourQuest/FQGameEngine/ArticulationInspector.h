#pragma once

#include "IEditorWindow.h"

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

#include <memory>

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;
	
	class ArticulationInspector : public IEditorWindow
	{
	public:
		ArticulationInspector();
		~ArticulationInspector();

		void Initialize(GameProcess* game, EditorProcess* editor);
		virtual void Render() override;


	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		bool mbIsOpen;

		std::shared_ptr<fq::game_module::GameObject> mSelectObject;

		fq::game_module::EventHandler mSelectObjectHandler;
	};
}
