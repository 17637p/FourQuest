#pragma once

#include "../FQGameModule/IStateBehaviour.h"
#include "PlayerDefine.h"

namespace fq::client
{
	class MagicBallAttackState :public game_module::IStateBehaviour
	{
	public:
		MagicBallAttackState();
		~MagicBallAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mAttackTiming; // 공격발동시간
		float mElapsedTime;

		friend void RegisterMetaData();
	};
}