#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BossMonsterContinousState : public fq::game_module::IStateBehaviour
	{
		enum class EAttackType
		{
			First, // 첫번째 공격
			Second, // 두번째 공격
			InitReady, // 루프초기화 대기
		};

	public:
		BossMonsterContinousState();
		~BossMonsterContinousState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mAttackDuration;
		float mAttackElapsedTime;
		float mFirstAttackTime;
		float mSecondAttackTime;
		EAttackType mAttackType;

		friend void RegisterMetaData();
	};


}