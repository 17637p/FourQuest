#pragma once
#include "IEditorWindow.h"

#include <memory>
#include <filesystem>
#include <imgui-node-editor/imgui_node_editor.h>

#include "../FQGameModule/GameModule.h"
#include "EditorEvent.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// Animator를 편집하는 창
	/// </summary>S
	class AnimatorWindow : public IEditorWindow
	{
		using NodeID = unsigned int;
		using PinID = unsigned int;
		using LinkID = unsigned int;
		using LinkPair = std::pair<std::string, std::string>;

		using EventHandler = fq::game_module::EventHandler;
	public:
		AnimatorWindow();
		~AnimatorWindow();

		void Initialize(GameProcess* game, EditorProcess* editor);

		void Finalize();

		void Render()override;

		bool& IsWindowOpen() { return mbIsOpen; }

		void OnStartScene();

		void OnUnloadScene();

		void SelectObject(fq::editor_event::SelectObject event);

		void SaveAnimatorController();

	private:
		void dragDropWindow();

		// 파라미터 관련
		void beginChild_ParameterWindow();
		void beginCombo_AddParameter();

		// 노드관련
		void beginChild_NodeEditor();
		void beginPopupContextWindow_NodeEditor();
		void beginNode_AnimationStateNode(const std::string& name
			, const fq::game_module::AnimationStateNode& node);
		void beginPin_AnimationStateNode(const std::string& nodeName
			, fq::game_module::AnimationStateNode::Type type);
		void beginLink_AnimationTransition(const fq::game_module::AnimationTransition& transition, bool onFlow);

		void beginCreate();
		void beginDelete();

		PinID getInputPinID(const std::string& nodeName);
		PinID getOutputPinID(const std::string& nodeName);

		void createContext();
		void destroyContext();
	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::EventManager* mEventManager;

		fq::game_module::AnimatorControllerLoader mLoader;
		std::string mSelectObjectName;
		std::filesystem::path mSelectControllerPath;
		std::shared_ptr<fq::game_module::AnimatorController> mSelectController;
		std::string mSettingFilePath;
		ax::NodeEditor::EditorContext* mContext;
		entt::id_type mSelectNodeID;

		bool mbIsOpen;
		std::unordered_map<NodeID, std::string> mMatchNodeID;
		std::unordered_map<PinID, std::string> mMatchPinID;
		std::unordered_map<LinkID, LinkPair> mMatchLinkID;

		EventHandler mOnLoadSceneHandler;
		EventHandler mStartSceneHandler;
		EventHandler mSelectObjectHandler;
	};


}