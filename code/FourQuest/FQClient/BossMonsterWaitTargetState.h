#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	/// <summary>
	/// 플레이어가 모두 영혼화상태여서 기다리는 상태
	/// </summary>
	class BossMonsterWaitTargetState : public fq::game_module::IStateBehaviour
	{
	public:
		BossMonsterWaitTargetState();
		~BossMonsterWaitTargetState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state , float dt) override;
		//void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
	};
}