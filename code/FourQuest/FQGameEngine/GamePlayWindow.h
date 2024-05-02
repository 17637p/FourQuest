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
		/// <param name="game"></param>
		/// <param name="editor"></param>
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

		// ������ ī�޶�
		ImVec2 mWindowSize;
		std::shared_ptr<fq::game_module::GameObject> mCameraObject;
		float mCameraMoveSpeed;
		float mCameraRotateSpeed;
		DirectX::SimpleMath::Matrix mViewTM;
		DirectX::SimpleMath::Matrix mProjTM;

		// ��������
		DirectX::SimpleMath::Matrix mStart;
		bool mbIsUsingGizumo;
		std::shared_ptr<fq::game_module::GameObject> mSelectObject;
		ImGuizmo::OPERATION mOperation;

		// �̺�Ʈ �ڵ�
		fq::game_module::EventHandler mSelectObjectHandler;
	};


}