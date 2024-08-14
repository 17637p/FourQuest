#pragma once

#include "IEditorWindow.h"

#include <memory>
#include <imgui.h>
#include <ImGuizmo.h>
#include <string>

#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class EventManager;
	class GameObject;
}

namespace fq::game_engine
{
	class Scene;
	class GameProcess;
	class EditorProcess;

	class ClothEditorWindow : public IEditorWindow
	{
	public:
		ClothEditorWindow();
		~ClothEditorWindow();

		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// ���������� â�� �����մϴ�
		/// </summary>
		void Finalize();

		virtual void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		/// <summary>
		/// ��ũ �����͸� �巡�� ����Ͽ� ���������� ��ġ�� �Լ�
		/// </summary>
		void dragDropGameObject();

		/// <summary>
		/// ���� ������ Ʈ���̾ޱ� �޽� �ݶ��̴��� �����Ѵ�.
		/// </summary>
		void createTriangle();

		/// <summary>
		/// Ʈ���̾ޱ� �޽� ����׸� ��ο��Ѵ�.
		/// </summary>
		void debugDrawTriangle();

	private:
		Scene* mScene;
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		bool mbIsOpen;

		fq::game_module::EventManager* mEventManager;

		std::shared_ptr<fq::game_module::GameObject> mGameObject;
	}; 

}