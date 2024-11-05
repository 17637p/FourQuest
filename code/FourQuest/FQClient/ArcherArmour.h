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
	class AimAssist;

	enum class EArcherSound
	{
		ShootStart,
		Charge1,
		Charge2,
		Shoot,
		Fastshoot1,
		Fastshoot2,
		Fastshoot3,
		Rolling,
	};

	class ArcherArmour : public game_module::Component
	{
	public:
		ArcherArmour();
		~ArcherArmour();

		void EmitmWeakAttack();
		void EmitStrongAttack(int chargeLevel);
		void EmitSound(EArcherSound archerSound);
		std::shared_ptr<game_module::GameObject> EmitChargingEffect();
		std::shared_ptr<game_module::GameObject> EmitStrongAttackEffect();
		std::shared_ptr<game_module::GameObject> EmitDash();

		/// <summary>
		/// 가까운 몬스터에게 에임을 보정합니다.
		/// </summary>
		void AimToNearMonster();

		/// <summary>
		/// 차지 레벨에 따른 화살의 방향들을 반환합니다 
		/// </summary>
		std::vector<DirectX::SimpleMath::Quaternion> GetStrongArrowDirections(int chargeLevel);

		/// <summary>
		/// 조준선을 생성합니다.
		/// </summary>
		void MakeLineOfSight();

		void RemoveLineOfSight();

	private:
		void OnStart() override;
		void OnUpdate(float dt) override;
		void checkSkillCoolTime(float dt);
		void checkInput(float dt);
		void setName();
		void makeStrongAttackArrow(float damage, DirectX::SimpleMath::Quaternion direction, DirectX::SimpleMath::Vector3 position
		, std::function<void()> hitCallback);

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		game_module::Animator*				mAnimator;
		game_module::CharacterController*	mController;
		game_module::Transform*				mTransform;
		game_module::Transform*				mWeaponeSocketT;
		client::Player*						mPlayer;
		AimAssist*							mAimAssist;

		float mDashCoolTime;
		float mDashCoolTimeReduction;
		float mDashElapsedTime;
		float mStrongAttackCoolTime;
		float mStrongAttackCoolTimeReduction;
		float mStrongAttackElapsedTime;
		float mArrowPower;
		float mRStickNoInputTime;
		float mWeakProjectileVelocity;
		float mStrongProjectileVelocity;
		float mStrongArrowOffset;
		float mStrongArrowRange;

		game_module::PrefabResource mWeakAttack;
		game_module::PrefabResource mStrongAttack;
		game_module::PrefabResource mStrongAttackChargingEffect;
		game_module::PrefabResource mStrongAttackLaunchEffect;
		game_module::PrefabResource mDashEffect;
		game_module::PrefabResource mLineOfSightEffect;
		game_module::GameObject* mLineOfSightObject;

		friend void RegisterMetaData();
	};
}