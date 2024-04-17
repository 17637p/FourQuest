#pragma once

#include "IEditorWindow.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// 윈도우 창아래에 생성된는 메뉴바 
	/// </summary>
	class MenuBar : public IEditorWindow
	{
	public:
		MenuBar();
		~MenuBar();

	public:
		void Initialize(GameProcess* game, EditorProcess* editor);

		void Render() override;

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
	};



}