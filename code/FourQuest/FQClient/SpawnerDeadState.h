#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class SpawnerDeadState : public fq::game_module::IStateBehaviour
	{
	public:
		SpawnerDeadState();
		~SpawnerDeadState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		virtual void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
	};
}
