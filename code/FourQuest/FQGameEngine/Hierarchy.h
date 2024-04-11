#pragma once
#include "IEditorWindow.h"

#include <memory>
#include <string>

namespace fq::game_module
{
	class Scene;
	class InputManager;
	class GameObject;
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
		void BeginHierarchy();

		/// <summary>
		/// �˻��Ҷ� ������ ���ӿ�����Ʈ ǥ�� â
		/// </summary>
		void BeginHierarchyOfSearch();

		/// <summary>
		/// ���ӿ�����Ʈ�� �̸��� ��Ÿ���� ��
		/// </summary>
		void BeginGameObjectNameBar(fq::game_module::GameObject& object);

		/// <summary>
		/// ���ӿ�����Ʈ�� ���³��� ��
		/// </summary>
		/// <param name="object">������ ǥ���ϴ� ������Ʈ</param>
		void BeginGameObjectBar(fq::game_module::GameObject& object);

		/// <summary>
		/// ���ӿ�����Ʈ ���ù�ư
		/// </summary>
		/// <param name="object">������Ʈ</param>
		void BegineGameObectSelectButton(fq::game_module::GameObject& object);

		void DragDropGameObject(fq::game_module::GameObject& object);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		fq::game_module::Scene* mScene;
		fq::game_module::InputManager* mInputManager;
		std::shared_ptr<fq::game_module::GameObject> mSelectObject;

		std::string mSearchString;

	};


}