#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "../FQGameModule/InputEnum.h"

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

	/// <summary>
	/// 마법 갑옷 
	/// </summary>
	class MagicArmour : public fq::game_module::Component
	{
	public:
		MagicArmour();
		~MagicArmour();

		/// <summary>
		/// X 버튼 마법 공격을 발사합니다
		/// </summary>
		void EmitMagicBall();

		/// <summary>r
		/// A 버튼 장판 마법 공격을 발사합니다
		/// </summary>
		void EmitAOE(DirectX::SimpleMath::Vector3 attackPoint);

		/// <summary>
		/// 레이저 기모으기 이펙트를 발산합니다 
		/// </summary>
		std::shared_ptr<fq::game_module::GameObject> EmitLaserGatherEffect();
		std::shared_ptr<fq::game_module::GameObject> EmitLaserHeadEffect();

		void EmitLaserLineEffect();
		void DestroyLaserEffect();

		/// <summary>
		/// R 스틱으로 Razer를 쏩니다
		/// </summary>
		void EmitLaser();

		fq::game_module::PrefabResource GetAttackWarningUI() const { return mAttackWarningUI; }

		float GetAOEMoveRange() const { return mAOEMoveRange; }

		void EnterLaserState();

		/// <summary>
		/// 가까운 몬스터에게 에임을 보정합니다.
		/// </summary>
		void AimToNearMonster();

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override { return *this; }
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnDestroy() override;
		void checkInput(float dt);
		void checkCoolTime(float dt);

		void setName();

	private:
		game_module::Transform* mTransform;
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;
		AimAssist* mAimAisst;
		Player* mPlayer;

		unsigned int mMagicBallPenetrationCount;
		float mMagicBallSpeed;
		float mAOEMoveRange;
		float mAOECoolTime;
		float mAOECoolTimeReduction;
		float mAOEElapsedTime;
		float mLaserCoolTime;
		float mLaserCoolTimeReduction;
		float mLaserElapsedTime;
		float mRStickNoInputTime;
		float mLaserDistance;
		float mLaserHiTick;
		float mLaserHitElapsedTime;

		EVibrationMode mLaserHitVibrationMode;
		float mLaserHitVibrationIntensity;
		float mLaserHitVibrationDuration;

		std::shared_ptr<game_module::GameObject> mLaserLineEffect;
		std::shared_ptr<game_module::GameObject> mLaserHeadEffect;

		game_module::PrefabResource mMagicBall;
		game_module::PrefabResource mAttackWarningUI;
		game_module::PrefabResource mAOE;
		game_module::PrefabResource mLaserHeadEffectPrefab;
		game_module::PrefabResource mLaserLineEffectPrefab;
		game_module::PrefabResource mLaserGatherEffect;
		game_module::PrefabResource mLaserAttackBox;

		friend void RegisterMetaData();
	};


}