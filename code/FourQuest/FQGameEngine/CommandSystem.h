#pragma once

#include <list>
#include <memory>

namespace fq::game_module
{
	class InputManager;
}

namespace fq::game_engine
{
	class ICommand;
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// Editor Command를 관리 
	/// </summary>
	class CommandSystem
	{
	public:
		CommandSystem();
		~CommandSystem();

		/// <summary>
		/// 명령 시스템을 초기화합니다
		/// </summary>
		/// <param name="game">게임프로세스</param>
		/// <param name="editor">에디터 프로세스</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// 명령어 단축키가 눌린지 확인합니다
		/// </summary>
		void Update();

		/// <summary>
		/// 새로운 명령을 추가합니다
		/// 새로운 명령은 명령 리스트에 추가된후 실행됩니다
		/// </summary>
		/// <param name="command">새로운 명령</param>
		void Push(std::unique_ptr<ICommand> command);

		void Clear();

	private:
		void excute();
		void undo();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::InputManager* mInputManager;

		std::list<std::unique_ptr<ICommand>> mCommandList;
		std::list<std::unique_ptr<ICommand>>::iterator mCommandOrder;
	};
}