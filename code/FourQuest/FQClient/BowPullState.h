#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BowPullState : public fq::game_module::IStateBehaviour
	{
	public:
		BowPullState();
		~BowPullState();

		virtual void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		virtual void OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt) override;
		virtual void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		virtual std::shared_ptr<IStateBehaviour> Clone() override;
		virtual entt::meta_handle GetHandle() override;

		friend void RegisterMetaData();
	};

}