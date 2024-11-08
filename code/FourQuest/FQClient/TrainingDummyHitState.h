#pragma once

#include "../FQGameModule/IStateBehaviour.h"


namespace fq::client
{
	class TrainingDummyHitState : public fq::game_module::IStateBehaviour
	{
	public:
		TrainingDummyHitState();

		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mElapsed;
		float mDuration;
		DirectX::SimpleMath::Color mHitColor;
		float mRimPow;
		float mRimIntensity;

		friend void RegisterMetaData();
	};


}