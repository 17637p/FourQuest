#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BowSoulAttackState : public fq::game_module::IStateBehaviour
	{
	public:
		BowSoulAttackState();
		~BowSoulAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mAttackElapsedTime;
	};


}