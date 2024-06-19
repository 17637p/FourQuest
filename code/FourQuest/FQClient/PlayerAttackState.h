#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class PlayerAttackState : public game_module::IStateBehaviour
	{
	public:
		PlayerAttackState();
		~PlayerAttackState();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

		float mAttackRebound; 

		friend void RegisterMetaData();
	};
}

