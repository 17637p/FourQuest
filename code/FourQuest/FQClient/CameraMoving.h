#pragma once

#include "../FQGameModule/GameModule.h"

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
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		float GetZoomMin() const { return mZoomMin; }
		void SetZoomMin(float val) { mZoomMin = val; }
		float GetZoomMax() const { return mZoomMax; }
		void SetZoomMax(float val) { mZoomMax = val; }

		float GetMoveSpeed() const { return mMoveSpeed; }
		void SetMoveSpeed(float val) { mMoveSpeed = val; }
	private:
		DirectX::SimpleMath::Vector3 GetCenterPoint() const;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		fq::game_module::Camera* mMainCamera;

		float mCurZoom;

		// �ν����� ���� ����
		float mMoveSpeed;

		float mZoomMin;
		float mZoomMax;
	};
}
