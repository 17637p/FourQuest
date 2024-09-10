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
	class BerserkerAttackState : public game_module::IStateBehaviour
	{
	public:
		BerserkerAttackState();
		~BerserkerAttackState() = default;

	private:
		std::shared_ptr<IStateBehaviour> Clone() override;
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		EBerserkerAttackType mAttackType;
		float mAttackMovement;
		float mElapsedTime;
		float mAttackTiming;
		DirectX::SimpleMath::Vector3 mColliderOffset;
		DirectX::SimpleMath::Vector3 mColliderScale;
		float mKnocBackPower;
		float mDestroyTime;

		friend void RegisterMetaData();
	};

}