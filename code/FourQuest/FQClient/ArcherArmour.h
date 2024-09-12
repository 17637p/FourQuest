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

		void EmitmWeakAttack();
		void EmitStrongAttack();
		std::shared_ptr<game_module::GameObject> EmitChargingEffect();
		std::shared_ptr<game_module::GameObject> EmitStrongAttackEffect();
		std::shared_ptr<game_module::GameObject> EmitDash();

		/// <summary>
		/// R 스틱의 입력방향으로 바라보는 방향을 설정합니다
		/// </summary>
		void SetLookAtRStickInput();
		void SetLookAtLStickInput();

		float GetChangeChargingTime() { return mChangeChargingTime; }
		float GetOriginCharacterMaxSpeed() { return mOriginCharacterMaxSpeed; }

	private:
		void OnStart() override;
		void OnUpdate(float dt) override;

		void checkSkillCoolTime(float dt);
		void checkInput(float dt);

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		game_module::Animator*				mAnimator;
		game_module::CharacterController*	mController;
		game_module::Transform*				mTransform;
		client::Player*						mPlayer;

		float mDashCoolTime;
		float mDashElapsedTime;
		float mStrongAttackCoolTime;
		float mStrongAttackElapsedTime;
		float mArrowPower;
		float mChangeChargingTime;
		float mRStickNoInputTime;
		float mWeakProjectileVelocity;
		float mStrongProjectileVelocity;
		float mOriginCharacterMaxSpeed;

		game_module::PrefabResource mWeakAttack;
		game_module::PrefabResource mStrongAttack;
		game_module::PrefabResource mStrongAttackChargingEffect;
		game_module::PrefabResource mStrongAttackLaunchEffect;
		game_module::PrefabResource mDashEffect;

		friend void RegisterMetaData();
	};
}