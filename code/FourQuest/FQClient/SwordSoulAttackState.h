#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::client
{
	class SwordSoulAttackState : public game_module::IStateBehaviour
	{
	public:
		SwordSoulAttackState();
		~SwordSoulAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mAttackTiming;
		float mElapsedTime;
		std::shared_ptr<fq::game_module::GameObject> mEffect;

		friend void RegisterMetaData();
	};


}