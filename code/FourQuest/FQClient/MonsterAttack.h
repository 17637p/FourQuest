#pragma once
#include "../FQGameModule/IStateBehaviour.h"
#include <directxtk/SimpleMath.h>

namespace fq::client
{
	class MonsterAttack : public fq::game_module::IStateBehaviour
	{
	public:
		MonsterAttack();
		~MonsterAttack();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

		void rotateToTarget(fq::game_module::Animator& animator);
		void attack(fq::game_module::Animator& animator);

	private:
		float mWaitTime;
		bool mIsTransition;
	};
}
