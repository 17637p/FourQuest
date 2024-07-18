#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class Player;

	/// <summary>
	/// ±â»ç °©¿Ê
	/// </summary>
	class KnightArmour : public fq::game_module::Component
	{
	public:
		KnightArmour();
		~KnightArmour();

		void EmitSwordAttack();
		void EmitShieldAttack();
		void EmitShieldDashAttack();

		float GetShieldDashPower() const { return mShieldDashPower; }
		float GetXAttackDashPower() const { return mXAttackDashPower; }
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

		float mDashCoolTime;
		float mDashElapsedTime;
		float mShieldDashPower;
		float mXAttackDashPower;
		float mSwordKnockBackPower;
		float mShieldKnockPower;

		float mAttackOffset;

		game_module::PrefabResource mSwordAttack;
		game_module::PrefabResource mShieldAttack;
		game_module::PrefabResource mDashAttack;

		friend void RegisterMetaData();
	};

}
