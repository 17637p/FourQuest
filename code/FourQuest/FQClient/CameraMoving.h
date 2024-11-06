#pragma once

#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class Camera;
	class Transform;
	class ScreenManager;
}

namespace fq::client
{
	enum class EDirection
	{
		Top,
		Left,
		Bottom,
		Right
	};

	class CameraMoving : public fq::game_module::Component
	{
	public:
		CameraMoving();
		~CameraMoving();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void InitCameraPos();
		DirectX::SimpleMath::Vector3 GetCenterCameraInWorld();

		void AddPlayerTransform(fq::game_module::Transform* player);
		void DeletePlayerTransform(fq::game_module::Transform* player);

		bool IsValid(int playerID, EDirection eDirection);

		void SetColliderRotation();

	private:
		entt::meta_handle GetHandle() override { return *this; }

		DirectX::SimpleMath::Vector3 getViewPos(DirectX::SimpleMath::Vector3 worldPos);
		DirectX::SimpleMath::Vector3 getProjPos(DirectX::SimpleMath::Vector3 worldPos);

		DirectX::SimpleMath::Vector3 getCenterPosInView(float dt);
		void chaseCenter(float dt);

		void Zoom(float dt);
		void zoomIn(float dt);
		void zoomOut(float dt);
		void restrcitPlayerMove();

	private:
		fq::game_module::Camera* mMainCamera;

		std::vector<fq::game_module::Transform*> mPlayerTransforms;

		DirectX::SimpleMath::Vector3 mPlayersCenterPoint;

		bool mIsFixed;

		float mCurZoom;
		bool mIsZoomIn;
		bool mIsZoomOut;

		// �ν����� ���� ����
		float mMoveSpeed;

		float mZoomSpeed;

		float mZoomMin;
		float mZoomMax;

		DirectX::SimpleMath::Vector2 mZoomOutPadX; // x�� - ����, y�� + ����
		DirectX::SimpleMath::Vector2 mZoomOutPadY; // x�� - ����, y�� + ����

		DirectX::SimpleMath::Vector2 mZoomInPadX; // x�� - ����, y�� + ����
		DirectX::SimpleMath::Vector2 mZoomInPadY; // x�� - ����, y�� + ����

		DirectX::SimpleMath::Vector2 mForbiddenAreaPaddingX; // Zoom �ƿ� ���� ū ���� 
		DirectX::SimpleMath::Vector2 mForbiddenAreaPaddingY;

		DirectX::SimpleMath::Vector2 mForbiddenViewPortX; // ������ viewportX ��ǥ // screenSize �ݿ�
		DirectX::SimpleMath::Vector2 mForbiddenViewPortY; // ������ viewportY ��ǥ // screenSize �ݿ�

		DirectX::SimpleMath::Vector2 mForbiddenViewPortXFix; // ����
		DirectX::SimpleMath::Vector2 mForbiddenViewPorYXFix; // ����

		float mWidth = 0;
		float mHeight = 0;

		game_module::ScreenManager* mScreenManager;

		friend void RegisterMetaData();
	};
}
