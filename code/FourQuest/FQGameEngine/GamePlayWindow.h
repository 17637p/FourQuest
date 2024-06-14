#pragma once

#include <imgui.h>
#include <ImGuizmo.h>

#include "../FQGameModule/GameModule.h"

#include "IEditorWindow.h"
#include "EditorEnum.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// ���� ȭ�� ǥ���ϴ� â�Դϴ�
	/// </summary>
	class GamePlayWindow : public IEditorWindow
	{
	public:
		GamePlayWindow();
		~GamePlayWindow();

		/// <summary>
		/// â�� �ʱ�ȭ�մϴ�
		/// </summary>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// ���� â�� �����մϴ�
		/// </summary>
		void Finalize();

		/// <summary>
		/// ���� â�� ǥ���մϴ� 
		/// </summary>
		void Render() override;

		/// <summary>
		/// ��带 �����մϴ�
		/// </summary>
		/// <param name="mode">���</param>
		void SetMode(EditorMode mode);

		/// <summary>
		/// �������� ��带 ��ȯ�մϴ�
		/// </summary>
		EditorMode GetMode()const;

		bool& IsWindowOpen() { return mbIsOpen; }

		/// <summary>
		/// ������ ī�޶� ������Ʈ�մϴ�
		/// </summary>
		void UpdateCamera(float dt);

		/// <summary>
		/// ������ ī�޶� Ÿ���� �ٶ󺾴ϴ�
		/// </summary>
		void LookAtTarget(DirectX::SimpleMath::Vector3 target);

		void ExcutShortcut();
	private:
		void resizeWindow(ImVec2 size);
		void pickObject();
		void checkMouse();

		void beginMenuBar_Control();
		void beginImage_GameScreen();

		void beginButton_SwapCamera();
		void beginButton_Play();
		void beginButton_Stop();
		void beginGizumo();
		void drawSelectObjectDebugInfomation();
	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		
		EditorMode mMode;
		bool mbIsOpen;
		bool mbIsPauseGame;
		bool mbIsMouseHoveredWindow;

		ImVec2 mWindowSize;
		ImVec2 mViewportSize;
		ImVec2 mViewPortOffset;
		ImVec2 mImagePos;
	
		// ������ ī�޶�
		std::shared_ptr<fq::game_module::GameObject> mCameraObject;
		float mCameraMoveSpeed;
		float mCameraRotateSpeed;

		// ��������
		DirectX::SimpleMath::Matrix mStart;
		bool mbIsUsingGizumo;
		ImGuizmo::OPERATION mOperation;
		std::shared_ptr<fq::game_module::GameObject> mSelectObject;

		// �̺�Ʈ �ڵ�
		fq::game_module::EventHandler mSelectObjectHandler;
	};


}