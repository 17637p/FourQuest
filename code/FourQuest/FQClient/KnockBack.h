#pragma once
#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class KnockBack : public fq::game_module::Component
	{
	public:
		KnockBack();
		~KnockBack();

		void Set(float power, DirectX::SimpleMath::Vector3 direction);

	private:
		void OnStart() override;
		void OnFixedUpdate(float dt) override;
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void updateVelocity(float dt);
	private:
		game_module::Transform* mTransform;

		DirectX::SimpleMath::Vector3 mVelocity;
		float mSpeed;
		float mFriction;

		friend void RegisterMetaData();
	};

}