#pragma once

#include "EBerserkerAttackType.h"
#include "../FQGameModule/IStateBehaviour.h"
#include "../FQGameModule/PrefabResource.h"
#include "BerserkerAttackState.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::client
{
	class BerserkerRushState : public BerserkerAttackState
	{
	public:
		BerserkerRushState() = default;
		~BerserkerRushState() = default;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mVelocity;
		float mAcceleration;
		float mAccumulatedTime;
		float mMaxVelocity;
		std::shared_ptr<fq::game_module::GameObject> mAttackObjectOrNull;

		friend void RegisterMetaData();
	};
}
