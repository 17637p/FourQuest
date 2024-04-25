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
	/// 게임 화면 표시하는 창입니다
	/// </summary>
	class GamePlayWindow : public IEditorWindow
	{
	public:
		GamePlayWindow();
		~GamePlayWindow();

		/// <summary>
		/// 창을 초기화합니다
		/// </summary>
		/// <param name="game"></param>
		/// <param name="editor"></param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// 게임 창을 종료합니다
		/// </summary>
		void Finalize();

		/// <summary>
		/// 단축키 관련 처리를 합니다 
		/// </summary>
		void ExcutShortcut();

		/// <summary>
		/// 게임 창을 표시합니다 
		/// </summary>
		void Render() override;

		/// <summary>
		/// 모드를 설정합니다
		/// </summary>
		/// <param name="mode">모드</param>
		void SetMode(EditorMode mode);

		/// <summary>
		/// 에디터의 모드를 반환합니다
		/// </summary>
		EditorMode GetMode()const;

		void UpdateCamera(float dt);
	private:
		void resizeWindow(ImVec2 size);
	
		void beginMenuBar_Control();
		void beginImage_GameScreen();

		void beginButton_SwapCamera();
		void beginButton_Play();
		void beginButton_Stop();
		void beginGizumo();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		
		EditorMode mMode;
		bool mbIsPauseGame;

		// 에디터 카메라
		ImVec2 mWindowSize;
		std::shared_ptr<fq::game_module::GameObject> mCameraObject;
		float mCameraMoveSpeed;
		float mCameraRotateSpeed;
		DirectX::SimpleMath::Matrix mViewTM;
		DirectX::SimpleMath::Matrix mProjTM;

		// 기즈모관련
		DirectX::SimpleMath::Matrix mStart;
		bool mbIsUsingGizumo;
		std::shared_ptr<fq::game_module::GameObject> mSelectObject;
		ImGuizmo::OPERATION mOperation;


		// 이벤트 핸들
		fq::game_module::EventHandler mSelectObjectHandler;
	};


}