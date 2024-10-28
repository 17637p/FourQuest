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
	/// ���� ���� 
	/// </summary>
	class MagicArmour : public fq::game_module::Component
	{
	public:
		MagicArmour();
		~MagicArmour();

		/// <summary>
		/// X ��ư ���� ������ �߻��մϴ�
		/// </summary>
		void EmitMagicBall();

		/// <summary>r
		/// A ��ư ���� ���� ������ �߻��մϴ�
		/// </summary>
		void EmitAOE(DirectX::SimpleMath::Vector3 attackPoint);

		/// <summary>
		/// ������ ������� ����Ʈ�� �߻��մϴ� 
		/// </summary>
		std::shared_ptr<fq::game_module::GameObject> EmitLaserGatherEffect();
		std::shared_ptr<fq::game_module::GameObject> EmitLaserHeadEffect();

		void EmitLaserLineEffect();
		void DestroyLaserEffect();

		/// <summary>
		/// R ��ƽ���� Razer�� ���ϴ�
		/// </summary>
		void EmitLaser();

		fq::game_module::PrefabResource GetAttackWarningUI() const { return mAttackWarningUI; }

		float GetAOEMoveRange() const { return mAOEMoveRange; }

		void EnterLaserState();

		/// <summary>
		/// ����� ���Ϳ��� ������ �����մϴ�.
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