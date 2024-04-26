#pragma once
#include "IEditorWindow.h"

#include <memory>
#include <string>

#include "../FQGameModule/GameModule.h"

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
		/// ���������� â�� �����մϴ�
		/// </summary>
		void Finalize();

		/// <summary>
		/// �������� â�� �׸��ϴ�
		/// </summary>
		void Render() override;

		/// <summary>
		/// ����Ű�� ó���մϴ�
		/// </summary>
		void ExcuteShortcut();

		bool& IsWindowOpen() { return mbIsOpen; }
	private:
		/// <summary>
		/// ���콺 ��Ŭ�� â
		/// ������Ʈ ����, ���� ��� ó��
		/// </summary>
		void beginPopupContextWindow_HierarchyChild();

		/// <summary>
		/// ������Ʈ�� ���콺 ��Ŭ���ϸ� ������ â
		/// </summary>
		/// <param name="object"></param>
		void beginPopupContextItem_GameObject(fq::game_module::GameObject& object);

		/// <summary>
		/// �˻��ϴ� â
		/// </summary>
		void beginSearchBar();

		/// <summary>
		/// ���� ���ӿ�����Ʈ���� ǥ���ϴ� â
		/// </summary>
		void beginHierarchy();

		/// <summary>
		/// �˻��Ҷ� ������ ���ӿ�����Ʈ ǥ�� â
		/// </summary>
		void beginHierarchyOfSearch();

		/// <summary>
		/// ���ӿ�����Ʈ�� �̸��� ��Ÿ���� ��
		/// </summary>
		void beginGameObjectNameBar(fq::game_module::GameObject& object);

		/// <summary>
		/// ���ӿ�����Ʈ�� ���³��� ��
		/// </summary>
		/// <param name="object">������ ǥ���ϴ� ������Ʈ</param>
		void beginGameObjectBar(fq::game_module::GameObject& object);

		/// <summary>
		/// ���ӿ�����Ʈ ���ù�ư
		/// </summary>
		/// <param name="object">������Ʈ</param>
		void begineGameObectSelectButton(fq::game_module::GameObject& object);

		/// <summary>
		/// �巡�� ��� ���ӿ�����Ʈ ó��
		/// </summary>
		/// <param name="object"></param>
		void dragDropGameObjectBar(fq::game_module::GameObject& object);

		/// <summary>
		/// �巡�� ��� ������ ó��
		/// </summary>
		void dragDropWindow();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		bool mbIsOpen;

		fq::game_module::Scene* mScene;
		fq::game_module::InputManager* mInputManager;
		fq::game_module::EventManager* mEventManager;
		std::shared_ptr<fq::game_module::GameObject> mSelectObject;
		std::shared_ptr<fq::game_module::GameObject> mCloneObject;

		std::string mSearchString;

		// �̺�Ʈ����
		fq::game_module::EventHandler mSelectObjectHandle;
	};


}