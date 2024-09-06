#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class ShieldDashState :	public game_module::IStateBehaviour
	{
	public:
		ShieldDashState();
		~ShieldDashState();
		std::shared_ptr<IStateBehaviour> Clone() override;

		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	};
}