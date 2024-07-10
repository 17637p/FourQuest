#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class MeleeMonsterWaitAttackState : public  game_module::IStateBehaviour
	{
	public:
		MeleeMonsterWaitAttackState();
		~MeleeMonsterWaitAttackState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
	};


}