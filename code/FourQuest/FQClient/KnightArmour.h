#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
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
		void EmitDashAttack();

		float GetDashPower() const { return mDashPower; }
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

		float mDashCoolTime;
		float mDashElapsedTime;
		float mDashPower;

		friend void RegisterMetaData();
	};

}
