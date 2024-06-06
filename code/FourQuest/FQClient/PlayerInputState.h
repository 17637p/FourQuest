#pragma once

#include "C:\Users\PC\Desktop\Project\code\FourQuest\FQGameModule\IStateBehaviour.h"

namespace fq::client
{

	class PlayerInputState :public game_module::IStateBehaviour
	{
	public:
		PlayerInputState();
		~PlayerInputState();

		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	};

}