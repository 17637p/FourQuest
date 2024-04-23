#pragma once


#include "IEditorWindow.h"
#include "EditorEnum.h"

class ID3D11ShaderResourceView;

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

		/// <summary>
		/// ���� â�� �����մϴ�
		/// </summary>
		void Finalize();

		/// <summary>
		/// ����Ű ���� ó���� �մϴ� 
		/// </summary>
		void ExcutShortcut();

		void Update();

		/// <summary>
		/// ���� â�� ǥ���մϴ� 
		/// </summary>
		void Render() override;

		/// <summary>
		/// ��带 �����մϴ�
		/// </summary>
		/// <param name="mode">���</param>
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