#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class ImageUI;
}

namespace fq::client
{
	class BGaugeUI : public fq::game_module::Component
	{
	public:
		BGaugeUI();
		~BGaugeUI();

		void SetVisible(bool isVisible);
		bool IsVisible()const { return mbIsVisible; }
		void SetRatio(float ratio);

	private:
		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		void setUIInfo();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		bool  mbIsVisible;
		float mGaugeRatio;
		DirectX::SimpleMath::Vector2 mWorldOffset;
		DirectX::SimpleMath::Vector2 mGaugeSize;

		game_module::Transform* mTransform;
		game_module::ImageUI*	mImageUI;
		game_module::Camera*	mMainCamera;


		friend void RegisterMetaData();
	};

}