#pragma once


#include "IEditorWindow.h"


namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// ���� ȭ�� ǥ���ϴ� â�Դϴ�
	/// </summary>
	class GamePlayWindow : public IEditorWindow
	{
	public:
		GamePlayWindow();
		~GamePlayWindow();

		/// <summary>
		/// â�� �ʱ�ȭ�մϴ�
		/// </summary>
		/// <param name="game"></param>
		/// <param name="editor"></param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		void Render() override;

		

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;


	};


}