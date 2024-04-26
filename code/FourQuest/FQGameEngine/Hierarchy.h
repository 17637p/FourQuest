#pragma once
#include "IEditorWindow.h"

#include <memory>
#include <string>

#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class Scene;
	class InputManager;
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// Scene 계층구조 정보를 표시하는 창
	/// </summary>
	class Hierarchy : public IEditorWindow
	{
	public:
		Hierarchy();
		~Hierarchy();

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
		/// 계층구조 창을 그립니다
		/// </summary>
		void Render() override;

		/// <summary>
		/// 단축키를 처리합니다
		/// </summary>
		void ExcuteShortcut();

		bool& IsWindowOpen() { return mbIsOpen; }
	private:
		/// <summary>
		/// 마우스 우클릭 창
		/// 오브젝트 생성, 삭제 명령 처리
		/// </summary>
		void beginPopupContextWindow_HierarchyChild();

		/// <summary>
		/// 오브젝트에 마우스 우클릭하면 나오는 창
		/// </summary>
		/// <param name="object"></param>
		void beginPopupContextItem_GameObject(fq::game_module::GameObject& object);

		/// <summary>
		/// 검색하는 창
		/// </summary>
		void beginSearchBar();

		/// <summary>
		/// 씬의 게임오브젝트들을 표시하는 창
		/// </summary>
		void beginHierarchy();

		/// <summary>
		/// 검색할때 나오는 게임오브젝트 표시 창
		/// </summary>
		void beginHierarchyOfSearch();

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
		/// 게임오브젝트 선택버튼
		/// </summary>
		/// <param name="object">오브젝트</param>
		void begineGameObectSelectButton(fq::game_module::GameObject& object);

		/// <summary>
		/// 드래그 드랍 게임오브젝트 처리
		/// </summary>
		/// <param name="object"></param>
		void dragDropGameObjectBar(fq::game_module::GameObject& object);

		/// <summary>
		/// 드래그 드랍 프리팹 처리
		/// </summary>
		void dragDropWindow();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		bool mbIsOpen;

		fq::game_module::Scene* mScene;
		fq::game_module::InputManager* mInputManager;
		fq::game_module::EventManager* mEventManager;
		std::shared_ptr<fq::game_module::GameObject> mSelectObject;
		std::shared_ptr<fq::game_module::GameObject> mCloneObject;

		std::string mSearchString;

		// 이벤트관련
		fq::game_module::EventHandler mSelectObjectHandle;
	};


}