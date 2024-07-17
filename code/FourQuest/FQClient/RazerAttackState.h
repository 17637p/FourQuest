#pragma once
#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class RazerAttackState : public fq::game_module::IStateBehaviour
	{
	public:
		RazerAttackState();
		~RazerAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
	private:
	};


}