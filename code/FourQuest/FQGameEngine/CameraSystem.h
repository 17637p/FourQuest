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
		/// ī�޶� �ý����� �ʱ�ȭ�մϴ�
		/// </summary>
		/// <param name="gameProcess"></param>
		void Initialize(GameProcess* gameProcess);

		/// <summary>
		/// �׷��Ƚ��� ���ε��� ī�޶� �����մϴ� 
		/// </summary>
		/// <param name="type"></param>
		void SetBindCamera(CameraType type);

		/// <summary>
		/// �׷��Ƚ��� ī�޶� ���ε� �մϴ�.
		/// </summary>
		void Update();

		/// <summary>
		/// Game�� MainCamera�� �����մϴ�
		/// </summary>
		/// <param name="camera">ī�޶�</param>
		void SetMainGameCamera(fq::game_module::Camera* camera);

		/// <summary>
		/// ������ ����ī�޶� �����ɴϴ�
		/// </summary>
		/// <returns>���� ī�޶�</returns>
		fq::game_module::Camera* FindMainCamera()const;

		/// <summary>
		/// �������� ī�޶� �����մϴ�
		/// </summary>
		/// <param name="camera">ī�޶�</param>
		void SetEditorCamera(fq::game_module::Camera* camera);

		/// <summary>
		/// ���� ����ϴ� ī�޶� Ÿ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>ī�޶� Ÿ��</returns>
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
