#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	/// <summary>
	/// 근접몬스터 추격 상태
	/// </summary>
	class MeleeMonsterChaseState :public fq::game_module::IStateBehaviour
	{
	public:
		MeleeMonsterChaseState();
		~MeleeMonsterChaseState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
	};
}

