#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class RotationAnimation : public game_module::Component
	{
	public:
		RotationAnimation() = default;
		~RotationAnimation() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		float mDuration;
		bool mbUseLooping;
		bool mbUseEuler;
		DirectX::SimpleMath::Vector3 mStartRotationInEuler;
		DirectX::SimpleMath::Vector3 mEndRotationInEuler;
		bool mbUseQuaternion;
		DirectX::SimpleMath::Quaternion mStartRotationInQuaternion;
		DirectX::SimpleMath::Quaternion mEndRotationInQuaternion;
		float mElapsed;

		friend void RegisterMetaData();
	};
}