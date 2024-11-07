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

		// UI 레이어를 설정합니다.
		void SetSortLayer(unsigned int val) { mSortLayer = val; }
	
	private:
		void OnLateUpdate(float dt) override;
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

		unsigned int mSortLayer;

		friend void RegisterMetaData();
	};

}
