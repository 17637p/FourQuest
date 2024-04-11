#pragma once
#include "IEditorWindow.h"

#include <memory>
#include <string>

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
		/// 계층구조 창을 그립니다
		/// </summary>
		void Render() override;

	private:
		/// <summary>
		/// 마우스 우클릭하면 나오는 창
		/// </summary>
		void BeginPopupContextWindow();

		/// <summary>
		/// 검색하는 창
		/// </summary>
		void BeginSearchBar();

		/// <summary>
		/// 씬의 게임오브젝트들을 표시하는 창
		/// </summary>
		void BeginHierarchy();

		/// <summary>
		/// 검색할때 나오는 게임오브젝트 표시 창
		/// </summary>
		void BeginHierarchyOfSearch();

		/// <summary>
		/// 게임오브젝트의 이름을 나타내는 바
		/// </summary>
		void BeginGameObjectNameBar(fq::game_module::GameObject& object);

		/// <summary>
		/// 게임오브젝트를 나태내는 바
		/// </summary>
		/// <param name="object">정보를 표시하는 오브젝트</param>
		void BeginGameObjectBar(fq::game_module::GameObject& object);

		/// <summary>
		/// 게임오브젝트 선택버튼
		/// </summary>
		/// <param name="object">오브젝트</param>
		void BegineGameObectSelectButton(fq::game_module::GameObject& object);

		void DragDropGameObject(fq::game_module::GameObject& object);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		fq::game_module::Scene* mScene;
		fq::game_module::InputManager* mInputManager;
		std::shared_ptr<fq::game_module::GameObject> mSelectObject;

		std::string mSearchString;

	};


}