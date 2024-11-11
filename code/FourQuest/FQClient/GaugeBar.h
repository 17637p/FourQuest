#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/EventHandler.h"

namespace fq::game_module
{
	class Transform;
	class ImageUI;
	class Camera;
}

namespace fq::client
{
	class GaugeBar : public game_module::Component
	{
	public:
		GaugeBar();
		~GaugeBar();
		GaugeBar(const GaugeBar& other);
		GaugeBar& operator=(const GaugeBar& other);

		void SetVisible(bool isVisible);
		bool IsVisible()const { return mbIsVisible; }
		void SetRatio(float ratio);

	private:
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnDestroy() override;
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override { return *this; }
		void setUIInfo();

	private:
		bool mbIsVisible;
		bool mIsRenderingUI;
		game_module::EventHandler mUIRenderHandler;

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
