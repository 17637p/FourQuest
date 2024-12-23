#pragma once

#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class Transform;
	class ImageUI;
}

namespace fq::client
{
	/// <summary>
	/// HpBar UI는 ImageUI에 0,1,2 이미지를 사용합니다.
	/// </summary>
	class HpBar : public game_module::Component
	{
	public:
		HpBar();
		~HpBar();

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;

		void DecreaseHp(float ratio);
		void SetHp(float ratio);
		void SetVisible(bool isVisible);
		bool IsVisible()const { return mbIsVisible; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnUpdate(float dt) override;
		void setUIInfo();

	private:
		bool mbIsVisible;
		float mDecreaseRatio;
		float mDeceraseTime;
		float mHpRatio;

		DirectX::SimpleMath::Vector2 mBarSize;
		float mDecreaseSpeed;
		float mDecreaseOffset;
		float mWorldOffset;
		float mScreenOffset;
		float mInnerOffset;

		game_module::Transform* mTransform;
		game_module::ImageUI* mImageUI;

		friend void RegisterMetaData();
	};


}