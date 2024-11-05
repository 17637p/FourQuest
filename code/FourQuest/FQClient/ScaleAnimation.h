#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class ScaleAnimation : public game_module::Component
	{
	public:
		ScaleAnimation();
		~ScaleAnimation() = default;

		void SetPlaySpeed(float playSpeed) { mPlaySpeed = playSpeed; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		float mDuration;
		DirectX::SimpleMath::Vector3 mStartScale;
		DirectX::SimpleMath::Vector3 mEndScale;
		float mElapsed;
		float mPlaySpeed;

		friend void RegisterMetaData();
	};
}