#pragma once

#include "EBerserkerAttackType.h"
#include "../FQGameModule/IStateBehaviour.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::client
{
	class BerserkerRushChargingState : public game_module::IStateBehaviour
	{
	public:
		BerserkerRushChargingState();
		~BerserkerRushChargingState() = default;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mChargingTime;
		float mChargingMinimumTime;
		float mChargingElapsedTime;
		bool mbPassedPoint;

		friend void RegisterMetaData();
	};
}