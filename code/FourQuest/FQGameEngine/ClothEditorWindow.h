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
		/// 계층구조초 창을 종료합니다
		/// </summary>
		void Finalize();

		virtual void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		/// <summary>
		/// 링크 데이터를 드래그 드랍하여 계층구조를 고치는 함수
		/// </summary>
		void dragDropGameObject();

		/// <summary>
		/// 물리 엔진에 트라이앵글 메쉬 콜라이더를 생성한다.
		/// </summary>
		void createTriangle();

		/// <summary>
		/// 트래이앵글 메쉬 디버그를 드로우한다.
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