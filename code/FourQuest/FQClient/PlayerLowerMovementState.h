#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	/// <summary>
	/// 플레이어 하체 움직임 
	/// </summary>
	class PlayerLowerMovementState : public game_module::IStateBehaviour
	{
	public:
		PlayerLowerMovementState();
		~PlayerLowerMovementState();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

	private:
		bool mbOnEnterToLowerMovement;
		bool mbOffExitToLowerMovement;

		friend void RegisterMetaData();
	};

}
