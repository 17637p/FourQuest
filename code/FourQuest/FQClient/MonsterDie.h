#pragma once
#include "../FQGameModule/IStateBehaviour.h"
#include <directxtk/SimpleMath.h>

namespace fq::client
{
	class MonsterDie : public fq::game_module::IStateBehaviour
	{
	public:
		MonsterDie();
		~MonsterDie();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
	};
}