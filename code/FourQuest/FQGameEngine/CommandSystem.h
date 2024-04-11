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
	/// Editor Command�� ���� 
	/// </summary>
	class CommandSystem
	{
	public:
		CommandSystem();
		~CommandSystem();

		/// <summary>
		/// ��� �ý����� �ʱ�ȭ�մϴ�
		/// </summary>
		/// <param name="game">�������μ���</param>
		/// <param name="editor">������ ���μ���</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// ��ɾ� ����Ű�� ������ Ȯ���մϴ�
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
		// ���ο� ����� �������Ƿ� ������ �ִ� ����� ����ϴ�
		mCommandOrder = mCommandList.erase(mCommandOrder, mCommandList.end());

		// ���ο� ����� �߰��մϴ�.
		mCommandOrder = mCommandList.insert(mCommandOrder
			, std::make_unique<T>(std::forward<Args>(args)...));

		// ����� �����մϴ�
		(*mCommandOrder)->Excute();

		// ������ end�Դϴ�
		mCommandOrder = mCommandList.end();
	}

}