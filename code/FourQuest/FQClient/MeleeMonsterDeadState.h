#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class MeleeMonsterDeadState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterDeadState();
		~MeleeMonsterDeadState();

		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mDurationTime;
		float mEraseTime;

		friend void RegisterMetaData();
	};
}

