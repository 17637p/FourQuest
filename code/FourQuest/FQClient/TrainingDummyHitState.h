#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class TrainingDummyHitState : public fq::game_module::IStateBehaviour
	{
	public:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mDurationTime = 0.f;
	};


}