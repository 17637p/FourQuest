#pragma once
#include "IEditorWindow.h"

#include <memory>
#include <imgui-node-editor/imgui_node_editor.h>

#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// Animator를 편집하는 창
	/// </summary>
	class AnimatorWindow : public IEditorWindow
	{
	public:
		AnimatorWindow();
		~AnimatorWindow();

		void Initialize(GameProcess* game, EditorProcess* editor);

		void Finalize();

		void Render()override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		// 파라미터 관련
		void beginChild_ParameterWindow();
		void beginCombo_AddParameter();

		// 노드관련
		void beginChild_NodeEditor();
		void beginPopupContextWindow_NodeEditor();
		void beginNode_AnimationStateNode(const std::string& name
			, const fq::game_module::AnimationStateNode& node);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		std::shared_ptr<fq::game_module::AnimatorController> mSelectController;
		ax::NodeEditor::EditorContext* mContext;

		bool mbIsOpen;
	};


}