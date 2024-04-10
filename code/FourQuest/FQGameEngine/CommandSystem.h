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

		/// <summary>
		/// ���ο� ����� �߰��մϴ�
		/// ���ο� ����� ��� ����Ʈ�� �߰����� ����˴ϴ�
		/// </summary>
		/// <param name="command">���ο� ���</param>
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