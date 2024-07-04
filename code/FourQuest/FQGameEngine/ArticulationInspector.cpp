#include "ArticulationInspector.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQGameModule/Articulation.h"
#include "../FQGameModule/ArticulationData.h"

namespace fq::game_engine
{
	ArticulationInspector::ArticulationInspector()
	{
	}

	ArticulationInspector::~ArticulationInspector()
	{
	}

	void ArticulationInspector::Initialize(GameProcess* game, EditorProcess* editor)
	{
		mGameProcess = game;
		mEditorProcess = editor;

		mArticulationData = std::make_shared<fq::game_module::ArticulationData>();
	}

	void ArticulationInspector::Render()
	{
		if (!mbIsOpen) return;

		if (ImGui::Begin("Articulation Hierarchy", &mbIsOpen))
		{
			ImGui::BeginChild("PhysicsAnimatorTool");

			
		}
		ImGui::End();

	}

	void ArticulationInspector::createLink()
	{

	}
}
