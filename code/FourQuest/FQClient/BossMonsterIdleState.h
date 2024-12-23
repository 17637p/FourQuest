#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class BossMonsterIdleState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterIdleState();
		~BossMonsterIdleState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	};


}