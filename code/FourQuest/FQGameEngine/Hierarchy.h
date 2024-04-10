#pragma once
#include "IEditorWindow.h"

#include <string>

namespace fq::game_module
{
	class Scene;
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
		void BeginGameObjectBar();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		fq::game_module::Scene* mScene;

		std::string mSearchString;

	};


}