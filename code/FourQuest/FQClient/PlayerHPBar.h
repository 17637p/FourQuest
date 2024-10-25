#pragma once

#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class Transform;
	class ImageUI;
}

namespace fq::client
{
	class PlayerHPBar :public fq::game_module::Component
	{
	public:
		PlayerHPBar();
		~PlayerHPBar();

		void DecreaseHp(float ratio);
		void SetVisible(bool isVisible);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnUpdate(float dt) override;
		void OnAwake() override;

	private:
		bool mbIsVisible;
		float mHpRatio;
		float mDecreaseRatio;
		float mDecreaseTime;
		float mWorldYOffset;
		float mScreenYOffset;

		DirectX::SimpleMath::Vector2 mInnerOffset;
		DirectX::SimpleMath::Vector2 mInnerBarSize;

		game_module::Transform* mTransform;
		game_module::ImageUI* mImageUI;

		friend void RegisterMetaData();
	};

}