#pragma once

#include <memory>
#include <imgui.h>
#include <ImGuizmo.h>

#include "IEditorWindow.h"

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class ArticulationData;
	class LinkData;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class ArticulationHierarchy : public IEditorWindow
	{
	public:
		ArticulationHierarchy();
		~ArticulationHierarchy();

		/// <summary>
		/// 계층구조 창을 초기화합니다
		/// </summary>
		/// <param name="process">게임프로세스</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// 계층구조초 창을 종료합니다
		/// </summary>
		void Finalize();

		/// <summary>
		/// 관절 에디터 계층구조 창을 그립니다.
		/// </summary>
		virtual void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		/// <summary>
		/// 계층구조 버튼 함수 (본과 Articulation 계층구조 분리)
		/// </summary>
		void beginHierarchyButton();

		/// <summary>
		/// 드래그 드랍 게임오브젝트 처리 (model 데이터를 로드해서 본 데이터 추가)
		/// </summary>
		void dragDropGameWindow();

		/// <summary>
		/// 씬의 본 오브젝트들을 표시하는 창
		/// </summary>
		void beginBoneHierarchy();

		/// <summary>
		/// 게임오브젝트(본)의 이름을 나타내는 바
		/// </summary>
		void beginGameObjectNameBar(fq::game_module::GameObject& object);

		/// <summary>
		/// 게임오브젝트(본)를 나태내는 바
		/// </summary>
		/// <param name="object">정보를 표시하는 오브젝트</param>
		void beginGameObjectBar(fq::game_module::GameObject& object);

		/// <summary>
		/// 관절의 계층구조(Articulation)를 표시하는 창
		/// </summary>
		void beginArticulationHierarchy();

		/// <summary>
		/// Articulation의 Link들을 표시하는 창
		/// </summary>
		void beginLinkDataBar(std::shared_ptr<fq::game_module::LinkData> link);

		/// <summary>
		/// Articulation의 Link의 이름을 표시하는 바
		/// </summary>
		void beginLinkDataNameBar(std::shared_ptr<fq::game_module::LinkData> link);

		/// <summary>
		/// 선택한 Link의 버튼을 관리하는 바
		/// </summary>
		void beginLinkSelectButton(std::shared_ptr<fq::game_module::LinkData> link);

		/// <summary>
		/// 자식 Link클 생성하는 팝업창
		/// </summary>
		void beginPopupContextWindow_HierarchyChild();

		/// <summary>
		/// 관절의 계층 구조에서 우클릭 하면 링크를 생성할지 나타나는 팝업창
		/// </summary>
		void beginPopupContextItem_Link(std::shared_ptr<fq::game_module::LinkData> link);

		/// <summary>
		/// 링크 데이터를 드래그 드랍하여 계층구조를 고치는 함수
		/// </summary>
		void dragDropLinkBar(std::shared_ptr<fq::game_module::LinkData> link);

		/// <summary>
		/// 선택된 Link와 Joint에 Guizmo를 입히는 함수
		/// </summary>
		void beginGizmo();

		/// <summary>
		/// Articulation(관절) 디버깅
		/// </summary>
		void drawArticulationDebug(std::shared_ptr<fq::game_module::LinkData> linkData);

		/// <summary>
		/// 링크 디버깅
		/// </summary>
		void drawLinkDebug(std::shared_ptr<fq::game_module::LinkData> linkData, float& jointAxisScale);

		/// <summary>
		/// 조인트 회전축 디버깅
		/// </summary>
		void drawJointAxisDebug(std::shared_ptr<fq::game_module::LinkData> linkData, const float& jointAxisScale);

	private:
		GameProcess*	mGameProcess;
		EditorProcess*	mEditorProcess;

		bool			mbIsOpen;
		bool			mbIsBoneHierarchy;
		bool			mbIsArticulationHierarchy;

		fq::game_module::Scene* mScene;
		fq::game_module::EventManager* mEventManager;
		fq::game_module::InputManager* mInputManager;
		
		// 본 관련 ( 그래픽스 )
		std::filesystem::path* mPath;
		std::shared_ptr<fq::game_module::GameObject> mObject;
		fq::game_module::SkinnedMeshRenderer* mSkinnedMeshRenderer;
		std::vector<fq::common::Bone> mBones;

		// 관절 관련
		std::shared_ptr<fq::game_module::ArticulationData> mArticulationData;
		std::shared_ptr<fq::game_module::LinkData> mSelectLinkData;

		// 이벤트관련
		fq::game_module::EventHandler mSelectLinkHandle;

		unsigned int mLinkID;
	};
}