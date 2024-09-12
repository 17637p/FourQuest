#pragma once
#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BowStrongChargingState : public fq::game_module::IStateBehaviour
	{
	public:
		BowStrongChargingState();
		~BowStrongChargingState();

	private:
		virtual void OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;
		virtual void OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt) override;
		virtual void OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state) override;

		virtual std::shared_ptr<IStateBehaviour> Clone() override;
		virtual entt::meta_handle GetHandle() override;

	private:
		float mChargingElapsedTime;
		float mRotationSpeed;

		std::shared_ptr<game_module::GameObject> mChargingEffect;

		friend void RegisterMetaData();
	};
}