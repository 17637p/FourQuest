#pragma once


#include "IEditorWindow.h"


namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// 게임 화면 표시하는 창입니다
	/// </summary>
	class GamePlayWindow : public IEditorWindow
	{
	public:
		GamePlayWindow();
		~GamePlayWindow();

		/// <summary>
		/// 창을 초기화합니다
		/// </summary>
		/// <param name="game"></param>
		/// <param name="editor"></param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// 게임 창을 종료합니다
		/// </summary>
		void Finalize();

		void Render() override;

		

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;


	};


}