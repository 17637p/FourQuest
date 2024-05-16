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
	/// Animator�� �����ϴ� â
	/// </summary>
	class AnimatorWindow : public IEditorWindow
	{
		using PinID = unsigned int;
		using LinkID = unsigned int;
		using LinkPair = std::pair<std::string, std::string>;

	public:
		AnimatorWindow();
		~AnimatorWindow();

		void Initialize(GameProcess* game, EditorProcess* editor);

		void Finalize();

		void Render()override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		// �Ķ���� ����
		void beginChild_ParameterWindow();
		void beginCombo_AddParameter();

		// ������
		void beginChild_NodeEditor();
		void beginPopupContextWindow_NodeEditor();
		void beginNode_AnimationStateNode(const std::string& name
			, const fq::game_module::AnimationStateNode& node);
		void beginPin_AnimationStateNode(const std::string& nodeName
			, fq::game_module::AnimationStateNode::Type type);
		void beginLink_AnimationTransition(const fq::game_module::AnimationTransition& transition);
	
		void beginCreate();
		void beginDelete();

		PinID getInputPinID(const std::string& nodeName);
		PinID getOutputPinID(const std::string& nodeName);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::EventManager* mEventManager;

		std::shared_ptr<fq::game_module::AnimatorController> mSelectController;
		ax::NodeEditor::EditorContext* mContext;

		bool mbIsOpen;
		std::unordered_map<PinID, std::string> mMatchPinID;
		std::unordered_map<LinkID, LinkPair> mMatchLinkID;
	};


}