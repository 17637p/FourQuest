#pragma once


namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// Prefab의 연결 정보를 관리하는 클래스
	/// </summary>
	class PrefabSystem
	{
	public:
		PrefabSystem();
		~PrefabSystem();

		/// <summary>
		/// 프리팹 시스템을 초기화합니다 
		/// </summary>
		/// <param name="game">게임 프로세스</param>
		/// <param name="editor">에디터 프로세스</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// 프리팹 시스템을 종료합니다
		/// </summary>
		void Finalize();

	private:


	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;


	};


}