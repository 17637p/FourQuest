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
	class GaugeBar;

	/// <summary>
	/// ��� ����
	/// </summary>
	class KnightArmour : public fq::game_module::Component
	{
	public:
		KnightArmour();
		~KnightArmour();

		std::shared_ptr<game_module::GameObject> EmitSwordEffect();

		void EmitSwordAttack();
		void EmitShieldAttack();
		void EmitShieldDashAttack();

		/// <summary>
		/// ��� ���¿� ���� ó�����մϴ�.
		/// </summary>
		void CheckBlockState(float dt);

		float GetShieldDashPower() const { return mShieldDashPower; }
		float GetXAttackDashPower() const { return mXAttackDashPower; }

		/// <summary>
		/// ������ �̵��ӵ��� �����մϴ�
		/// </summary>
		void SetShieldMovementSpeed(bool isShieldSpeed);

		void ExitShieldState();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnUpdate(float dt) override;
		void OnStart() override;

		void checkSkillCoolTime(float dt);
		void checkInput();

	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;
		game_module::Transform* mTransform;
		client::Player* mPlayer;
		client::GaugeBar* mGaugeBar;

		float mDashCoolTime;
		float mDashElapsedTime;
		float mShieldCoolTime;
		float mShieldSpeedRatio;
		float mShieldDuration;
		float mShieldElapsedTime;

		float mShieldDashPower;
		float mXAttackDashPower;
		float mSwordKnockBackPower;
		float mDashKnockBackPower;

		float mAttackOffset;

		game_module::PrefabResource mSwordAttack;
		game_module::PrefabResource mSwordAttackEffect1;
		game_module::PrefabResource mSwordAttackEffect2;
		game_module::PrefabResource mShieldAttack;
		game_module::PrefabResource mDashAttack;

		friend void RegisterMetaData();
	};

}
