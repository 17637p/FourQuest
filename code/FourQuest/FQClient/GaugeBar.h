#pragma once

#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class Transform;
	class ImageUI;
	class Camera;
}

namespace fq::client
{
	/// <summary>
	/// 게이지 UI 3,4 이미지를 사용합니다 
	/// </summary>
	class GaugeBar : public game_module::Component
	{
	public:
		GaugeBar();
		~GaugeBar();

		void SetVisible(bool isVisible);
		bool IsVisible()const { return mbIsVisible; }
		void SetRatio(float ratio);

	private:
		void OnStart() override;
		void OnUpdate(float dt) override;
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override { return *this; }
		void setUIInfo();

	private:
		bool mbIsVisible;

		DirectX::SimpleMath::Vector2 mBarSize;
		float mWorldOffset;
		float mScreenOffset;
		float mInnerOffset;
		float mGaugeRatio;

		game_module::Transform* mTransform;
		game_module::ImageUI* mImageUI;

		friend void RegisterMetaData();
	};

}
