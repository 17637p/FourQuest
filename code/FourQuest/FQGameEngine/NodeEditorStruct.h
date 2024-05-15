#pragma once

#include <imgui-node-editor/imgui_node_editor.h>

namespace fq::game_engine
{

	struct Link
	{
		ax::NodeEditor::LinkId id;
		ax::NodeEditor::PinId input;
		ax::NodeEditor::PinId output;
	};
}
