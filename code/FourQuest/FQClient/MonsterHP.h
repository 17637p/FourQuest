#pragma once

#include "../FQGameModule/Component.h"


namespace fq::game_module
{
	class Transform;
	class ImageUI;
}

namespace fq::client
{
	class MonsterHP : public game_module::Component
	{
	public:
		MonsterHP();
		~MonsterHP();

		void DecreaseHp(float ratio);
		void SetVisible(bool isVisible);

	private:
		void OnUpdate(float dt) override;
		void OnStart() override;
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mHpRatio; 
		float mDecreaseRatio;
		float mDecreaseTime;
		float mDecreaseSpeed;

		float mbIsVisible;
		float mWorldYOffset;
		float mScreenYOffset;
		DirectX::SimpleMath::Vector2 mBarSize;
		DirectX::SimpleMath::Vector2 mInnerOffset;
		DirectX::SimpleMath::Vector2 mRankOffset;

		game_module::Transform* mTransform;
		game_module::ImageUI* mImageUI;

		friend void RegisterMetaData();
	};

}
