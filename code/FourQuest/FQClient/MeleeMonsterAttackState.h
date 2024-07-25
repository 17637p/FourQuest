#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::client
{
	/// <summary>
	/// 근접 몬스터 공격 상태
	/// </summary>
	class MeleeMonsterAttackState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterAttackState();
		~MeleeMonsterAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;	
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		float mAttackTiming; // 공격발동 시간
		float mElapsedTime;


		std::shared_ptr<game_module::GameObject> mEffect;

		friend void RegisterMetaData();
	};

}
