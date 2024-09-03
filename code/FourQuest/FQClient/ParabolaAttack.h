#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	/// <summary>
	/// 포물선 공격 
	/// </summary>
	class ParabolaAttack : public game_module::Component
	{
	public:
		ParabolaAttack();
		~ParabolaAttack();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnFixedUpdate(float dt) override;
		void OnStart() override;

 	private:
		DirectX::SimpleMath::Vector3 mVelocity;
		float mMoveDistance;
	};

}
