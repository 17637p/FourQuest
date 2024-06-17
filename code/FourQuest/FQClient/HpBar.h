#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class HpBar : public game_module::Component
	{
	public:
		HpBar();
		~HpBar();

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;

		bool IsVisible() const { return mIsVisible; }
		void SetVisible(bool isVisible);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnUpdate(float dt) override;

	private:
		bool mIsVisible;
		float mBarLength; // 0.f ~ 1.f
		float mDecreaseSpeed;

		game_module::Transform* mTransform;
		game_module::ImageUI* mImageUI;
		game_module::Camera* mMainCamera;
	};


}