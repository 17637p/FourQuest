#pragma once
#include "IEditorWindow.h"

#include <string>

namespace fq::game_module
{
	class Scene;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// Scene �������� ������ ǥ���ϴ� â
	/// </summary>
	class Hierarchy : public IEditorWindow
	{
	public:
		Hierarchy();
		~Hierarchy();

		/// <summary>
		/// �������� â�� �ʱ�ȭ�մϴ�
		/// </summary>
		/// <param name="process">�������μ���</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// �������� â�� �׸��ϴ�
		/// </summary>
		void Render() override;

	private:
		/// <summary>
		/// ���콺 ��Ŭ���ϸ� ������ â
		/// </summary>
		void BeginPopupContextWindow();

		/// <summary>
		/// �˻��ϴ� â
		/// </summary>
		void BeginSearchBar();

		/// <summary>
		/// ���� ���ӿ�����Ʈ���� ǥ���ϴ� â
		/// </summary>
		void BeginGameObjectBar();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		fq::game_module::Scene* mScene;

		std::string mSearchString;

	};


}