#pragma once


#include "IEditorWindow.h"
#include "EditorEnum.h"

class ID3D11ShaderResourceView;

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

		/// <summary>
		/// 단축키 관련 처리를 합니다 
		/// </summary>
		void ExcutShortcut();

		void Update();

		/// <summary>
		/// 게임 창을 표시합니다 
		/// </summary>
		void Render() override;

		/// <summary>
		/// 모드를 설정합니다
		/// </summary>
		/// <param name="mode">모드</param>
		void SetMode(EditorMode mode);

		EditorMode GetMode()const { return mMode; }


	private:
		void beginMenuBar_Control();
		
		void beginButton_Play();
		void beginButton_Stop();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		
		EditorMode mMode;
		bool mbIsPauseGame;

		ID3D11ShaderResourceView* mTexture;

	};


}