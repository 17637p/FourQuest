#pragma once
#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class BossMonsterPrepareAttackState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterPrepareAttackState();
		~BossMonsterPrepareAttackState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		
	public:
		float mHomingTime;
		float mHomingElapsedTime;

		friend void RegisterMetaData();
	};
}