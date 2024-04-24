#pragma once

#include "../FQGameModule/GameModule.h"

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
		/// 에디터의 카메라를 설정합니다
		/// </summary>
		/// <param name="camera">카메라</param>
		void SetEditorCamera(fq::game_module::Camera* camera);

	private:
		GameProcess* mGameProcess;
		CameraType mCameraType;

		fq::game_module::EventHandler mSetMainCameraHandler;
		fq::game_module::Camera* mGameMainCamera;
		fq::game_module::Camera* mEditorCamera;
	};

}
