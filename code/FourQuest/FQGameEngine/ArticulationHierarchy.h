#pragma once

#include <memory>

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
		/// 드래그 드랍 게임오브젝트 처리
		/// </summary>
		void dragDropGameWindow();

		/// <summary>
		/// 씬의 본 오브젝트들을 표시하는 창
		/// </summary>
		void beginBoneHierarchy();

		/// <summary>
		/// 게임오브젝트의 이름을 나타내는 바
		/// </summary>
		void beginGameObjectNameBar(fq::game_module::GameObject& object);

		/// <summary>
		/// 게임오브젝트를 나태내는 바
		/// </summary>
		/// <param name="object">정보를 표시하는 오브젝트</param>
		void beginGameObjectBar(fq::game_module::GameObject& object);

		/// <summary>
		/// 관절의 계층구조를 표시하는 창
		/// </summary>
		void beginArticulationHierarchy();

		/// <summary>
		/// 
		/// </summary>
		void beginPopupContextWindow_HierarchyChild();

		/// <summary>
		/// 관절의 계층 구조에서 우클릭 하면 링크를 생성할지 나타나는 창
		/// </summary>
		void beginPopupContextItem_Link();



	private:
		GameProcess*	mGameProcess;
		EditorProcess*	mEditorProcess;

		bool			mbIsOpen;
		bool			mbIsBoneHierarchy;
		bool			mbIsArticulationHierarchy;

		fq::game_module::Scene* mScene;
		fq::game_module::EventManager* mEventManager;
		fq::game_module::InputManager* mInputManager;
		
		std::filesystem::path* mPath;
		std::shared_ptr<fq::game_module::GameObject> mObject;
		fq::game_module::SkinnedMeshRenderer* mSkinnedMeshRenderer;
		std::vector<fq::common::Bone> mBones;

		std::shared_ptr<fq::game_module::ArticulationData> mArticulationData;
		std::vector<std::shared_ptr<fq::game_module::LinkData>> mLinkData;
		std::shared_ptr<fq::game_module::LinkData> mSelectLinkData;

		// 이벤트관련
		fq::game_module::EventHandler mSelectObjectHandle;
	};
}