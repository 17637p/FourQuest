#pragma once

#include "IEditorWindow.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// ������ â�Ʒ��� �����ȴ� �޴��� 
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