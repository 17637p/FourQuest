#pragma once
#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class BossMonsterGroggyState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterGroggyState();
		~BossMonsterGroggyState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mCollisionRadius;
		DirectX::SimpleMath::Vector3 mCollisionOffset;

		float mPrevCollisionRadius;
		DirectX::SimpleMath::Vector3 mPrevCollisionOffset;

		friend void RegisterMetaData();
	};
}