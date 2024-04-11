#pragma once

#include <list>
#include <memory>

#include "Command.h"

namespace fq::game_module
{
	class InputManager;
}

namespace fq::game_engine
{
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

		void Clear();

		template <typename T, typename... Args>
		void Push(Args&&... arg);
	private:
		void excute();
		void undo();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::InputManager* mInputManager;

		std::list<std::unique_ptr<Command>> mCommandList;
		std::list<std::unique_ptr<Command>>::iterator mCommandOrder;
	};

	template <typename T, typename...Args>
	void fq::game_engine::CommandSystem::Push(Args&&... args)
	{
		// 새로운 명령이 들어왔으므로 기존에 있던 명령을 지웁니다
		mCommandOrder = mCommandList.erase(mCommandOrder, mCommandList.end());

		// 새로운 명령을 추가합니다.
		mCommandOrder = mCommandList.insert(mCommandOrder
			, std::make_unique<T>(std::forward<Args>(args)...));

		// 명령을 실행합니다
		(*mCommandOrder)->Excute();

		// 순서는 end입니다
		mCommandOrder = mCommandList.end();
	}

}