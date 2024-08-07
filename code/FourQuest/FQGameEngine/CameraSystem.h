#pragma once

#include "../FQGameModule/EventHandler.h"

namespace fq::game_module
{
	class Camera;
}

namespace fq::game_engine
{
	class GameProcess;

	class CameraSystem
	{
	public:
		enum class CameraType
		{
			Game,
			Editor,
			None,
		};

	public:
		CameraSystem();
		~CameraSystem();

		/// <summary>
		/// 카메라 시스템을 초기화합니다
		/// </summary>
		/// <param name="gameProcess"></param>
		void Initialize(GameProcess* gameProcess);

		/// <summary>
		/// 그래픽스에 바인딩할 카메라를 설정합니다 
		/// </summary>
		/// <param name="type"></param>
		void SetBindCamera(CameraType type);

		/// <summary>
		/// 그래픽스에 카메라를 바인딩 합니다.
		/// </summary>
		void Update();

		/// <summary>
		/// Game의 MainCamera를 설정합니다
		/// </summary>
		/// <param name="camera">카메라</param>
		void SetMainGameCamera(fq::game_module::Camera* camera);

		/// <summary>
		/// 씬에서 메인카메라를 가져옵니다
		/// </summary>
		/// <returns>메인 카메라</returns>
		fq::game_module::Camera* FindMainCamera()const;

		/// <summary>
		/// 에디터의 카메라를 설정합니다
		/// </summary>
		/// <param name="camera">카메라</param>
		void SetEditorCamera(fq::game_module::Camera* camera);

		/// <summary>
		/// 현재 사용하는 카메라 타입을 반환합니다
		/// </summary>
		/// <returns>카메라 타입</returns>
		CameraType GetCameraType()const { return mCameraType; }

	private:
		GameProcess* mGameProcess;
		CameraType mCameraType;

		fq::game_module::EventHandler mSetMainCameraHandler;
		fq::game_module::EventHandler mGetMainCameraHandler;
		fq::game_module::EventHandler mOnLoadedSceneHandler;
		fq::game_module::EventHandler mSetViewportSizeHandler;
		fq::game_module::Camera* mGameMainCamera;
		fq::game_module::Camera* mEditorCamera;

		float mAspectRatio;
	};
}
