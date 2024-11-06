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
		/// 복사본을 반환합니다 
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

		// 인스펙터 공개 스탯
		float mMoveSpeed;

		float mZoomSpeed;

		float mZoomMin;
		float mZoomMax;

		DirectX::SimpleMath::Vector2 mZoomOutPadX; // x는 - 방향, y는 + 방향
		DirectX::SimpleMath::Vector2 mZoomOutPadY; // x는 - 방향, y는 + 방향

		DirectX::SimpleMath::Vector2 mZoomInPadX; // x는 - 방향, y는 + 방향
		DirectX::SimpleMath::Vector2 mZoomInPadY; // x는 - 방향, y는 + 방향

		DirectX::SimpleMath::Vector2 mForbiddenAreaPaddingX; // Zoom 아웃 보다 큰 영역 
		DirectX::SimpleMath::Vector2 mForbiddenAreaPaddingY;

		DirectX::SimpleMath::Vector2 mForbiddenViewPortX; // 금지할 viewportX 좌표 // screenSize 반영
		DirectX::SimpleMath::Vector2 mForbiddenViewPortY; // 금지할 viewportY 좌표 // screenSize 반영

		DirectX::SimpleMath::Vector2 mForbiddenViewPortXFix; // 기준
		DirectX::SimpleMath::Vector2 mForbiddenViewPorYXFix; // 기준

		float mWidth = 0;
		float mHeight = 0;

		game_module::ScreenManager* mScreenManager;

		friend void RegisterMetaData();
	};
}
