#pragma once
#include "../FQGameModule/IStateBehaviour.h"
#include <directxtk/SimpleMath.h>

namespace fq::client
{
	class MonsterDamaged : public fq::game_module::IStateBehaviour
	{
	public:
		MonsterDamaged();
		~MonsterDamaged();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
	};
}
