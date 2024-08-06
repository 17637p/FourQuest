#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BossMonsterRushState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterRushState();
		~BossMonsterRushState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		std::shared_ptr<game_module::GameObject> mRushHitBox;
	};
}	