#pragma once

#include "../FQGameModule/IStateBehaviour.h"
#include "PlayerDefine.h"

namespace fq::client
{
	/// <summary>
	/// 플레이어 공격상태에 대한 처리를 담당합니다
	/// </summary>
	class PlayerAttackState : public game_module::IStateBehaviour
	{
	public:
		PlayerAttackState();
		~PlayerAttackState();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mAttackRebound; // 공격 반동
		float mAttackTiming; // 공격 발동 시간
		float mElapsedTime;

		friend void RegisterMetaData();
	};
}

