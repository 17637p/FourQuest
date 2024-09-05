#pragma once

#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class Camera;
	class Transform;
}

namespace fq::client
{
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

		DirectX::SimpleMath::Vector3 GetCenterCameraInWorld();

		float GetMoveSpeed() const { return mMoveSpeed; }
		void SetMoveSpeed(float val) { mMoveSpeed = val; }

		float GetZoomSpeed() const { return mZoomSpeed; }
		void SetZoomSpeed(float val) { mZoomSpeed = val; }

		float GetZoomMin() const { return mZoomMin; }
		void SetZoomMin(float val) { mZoomMin = val; }
		float GetZoomMax() const { return mZoomMax; }
		void SetZoomMax(float val) { mZoomMax = val; }

		DirectX::SimpleMath::Vector2 GetZoomOutPadX() const { return mZoomOutPadX; }
		void SetZoomOutPadX(const DirectX::SimpleMath::Vector2& val) { mZoomOutPadX = val; }

		DirectX::SimpleMath::Vector2 GetZoomOutPadY() const { return mZoomOutPadY; }
		void SetZoomOutPadY(const DirectX::SimpleMath::Vector2& val) { mZoomOutPadY = val; }

		DirectX::SimpleMath::Vector2 GetZoomInPadX() const { return mZoomInPadX; }
		void SetZoomInPadX(const DirectX::SimpleMath::Vector2& val) { mZoomInPadX = val; }

		DirectX::SimpleMath::Vector2 GetZoomInPadY() const { return mZoomInPadY; }
		void SetZoomInPadY(const DirectX::SimpleMath::Vector2& val) { mZoomInPadY = val; }

		void AddPlayerTransform(fq::game_module::Transform* player);
		void DeletePlayerTransform(fq::game_module::Transform* player);

	private:
		entt::meta_handle GetHandle() override { return *this; }

		DirectX::SimpleMath::Vector3 getCenterPointInView(float dt);
		void chaseCenter(float dt);
		void zoomIn(float dt);
		void zoomOut(float dt);

	private:
		fq::game_module::Camera* mMainCamera;

		std::vector<fq::game_module::Transform*> mPlayerTransforms;

		DirectX::SimpleMath::Vector3 mPlayersCenterPoint;

		bool mIsFixed;
		//

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
	};
}
