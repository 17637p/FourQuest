#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class AOEAttackState :public fq::game_module::IStateBehaviour
	{
	public:
		AOEAttackState();
		~AOEAttackState();

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		fq::game_module::GameObject* mAttackWarningUI;


	};


}