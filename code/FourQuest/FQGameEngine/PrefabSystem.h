#pragma once


namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// Prefab�� ���� ������ �����ϴ� Ŭ����
	/// </summary>
	class PrefabSystem
	{
	public:
		PrefabSystem();
		~PrefabSystem();

		/// <summary>
		/// ������ �ý����� �ʱ�ȭ�մϴ� 
		/// </summary>
		/// <param name="game">���� ���μ���</param>
		/// <param name="editor">������ ���μ���</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// ������ �ý����� �����մϴ�
		/// </summary>
		void Finalize();

	private:


	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;


	};


}