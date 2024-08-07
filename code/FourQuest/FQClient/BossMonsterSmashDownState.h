#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BossMonsterSmashDownState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterSmashDownState();
		~BossMonsterSmashDownState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mEffectEmitTime;
		float mEffectElapsedTime;
		float mAttackEmitTime;
		float mAttackElapsedTime;
		float mHomingTime;
		float mHomingElapsedTime;

		std::shared_ptr<game_module::GameObject> mEffect;

		friend void RegisterMetaData();
	};
}

