#pragma once
#include "C:\Users\PC\Desktop\Project\code\FourQuest\FQGameModule\IStateBehaviour.h"

namespace fq::client
{
	class PlayerDashState :	public game_module::IStateBehaviour
	{
	public:
		PlayerDashState();
		~PlayerDashState();
		std::shared_ptr<IStateBehaviour> Clone() override;

		void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

		float GetDashPower() const { return mDashPower; }
		void SetDashPower(float power) { mDashPower = power; }
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mDashPower;
	};
}