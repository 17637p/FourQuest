#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::game_module
{
	class Animator;
	class Transform;
	class CharacterController;
}

namespace fq::client
{
	class Player;

	class ArcherArmour : public game_module::Component
	{
	public:
		ArcherArmour();
		~ArcherArmour();

		void EmitWeakAttack();
		void EmitStrongAttack();
		void EmitDash();

	private:
		void OnStart() override;
		void OnUpdate(float dt) override;

		void checkSkillCoolTime(float dt);
		void checkInput();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		game_module::Animator*				mAnimator;
		game_module::CharacterController*	mController;
		game_module::Transform*				mTransform;
		client::Player*						mPlayer;

		float mDashCoolTime;
		float mDashElapsedTime;
		float mArrowPower;

		game_module::PrefabResource mWeakAttack;
		game_module::PrefabResource mStrongAttack;
	};
}