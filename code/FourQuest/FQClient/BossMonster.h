#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "MonsterDefine.h"
#include "IMonster.h"

namespace fq::game_module
{
	class Transform;
	class Animator;
	class SkinnedMeshRenderer;
}

namespace fq::client
{
	class KnockBack;
	class GameManager;

	/// <summary>
	/// ������ ���� �������� Ŭ����
	/// </summary>
	class BossMonster : public game_module::Component, public IMonster
	{
	public:
		BossMonster();
		~BossMonster() = default;

		/// <summary>
		/// Ÿ���� ���� ������ �ִ��� Ȯ���մϴ� 
		/// </summary>
		void DetectTarget();

		/// <summary>
		/// ���� ���� ������ ������ �Ŀ� ȣ���մϴ�.
		/// </summary>
		void EndPattern();

		/// <summary>
		/// Ÿ���� �����մϴ� 
		/// </summary>
		void ChaseTarget();

		/// <summary>
		/// �÷��̾ ��ġ�� �°� 
		/// </summary>
		void HomingTarget(bool bUseSpeed = true);

		/// <summary>
		/// Ÿ���� ���ݹ����� �ִ��� Ȯ���մϴ�
		/// </summary>
		void CheckTargetInAttackRange();

		/// <summary>
		/// ���� �� �ִ� ���� ����� ������ Ž���ϰ� Ÿ������ �����մϴ�
		/// </summary>
		bool FindAndSetTargetDeadArmour();

		/// <summary>
		/// Ÿ���� �������� �����մϴ� 
		/// </summary>
		void SetRandomTarget();

		void Move(DirectX::SimpleMath::Vector3 destination);

		/// <summary>
		/// �޺�, ���Ӱ��� �� ������ �з����� �ݵ�
		/// </summary>
		void ReboundComboAttack();

		/// <summary>
		/// ���� ���� ���� ������ �ڷ� ��¦ �������� 
		/// </summary>
		void StepBack();

		void HitArrow(fq::game_module::GameObject* object);

		bool IsAngry() const;

		void ResetJumpCoolTime();
		void ResetRourCoolTime();

#pragma region HPBar
		void AddHp(float hp);

		/// <summary>
		/// HPBar�� �����մϴ�
		/// </summary>
		void CreateHpBar();

		/// <summary>
		/// HpBar�� �����մϴ�
		/// </summary>
		void DestroryHpBar();
#pragma endregion HPBar

#pragma region EmitEffect/Attack
		/// <summary>
		/// ������� ���� ����Ʈ�� �����մϴ� 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitSmashDownEffect();

		/// <summary>
		/// ������� ���� ��Į ����Ʈ�� �����մϴ�
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitSmashDecalEffect();

		void SetSmashDecalEffect(std::shared_ptr<game_module::GameObject> gameObject);

		/// <summary>
		/// �޺� ���� ����Ʈ�� �����մϴ� 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitComboEffect(float xAxisOffset);

		/// <summary>
		/// �޺� ���� ��Į ����Ʈ�� �����մϴ� 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitComboDecalEffect(float xAxisOffset);

		void SetComboDecalEffect(std::shared_ptr<game_module::GameObject> gameObject, float xAxisOffset);

		/// <summary>
		/// �뽬 ���� ����Ʈ�� �����մϴ�
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitRushEffect();

		/// <summary>
		/// �뽬 ���� ��Į ����Ʈ�� �����մϴ�
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitRushDecalEffect();

		/// <summary>
		/// ���� ���� ����Ʈ�� �����մϴ�
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitJumpEffect();

		/// <summary>
		/// ���� ������ ��Į ����Ʈ�� �����մϴ�
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitJumpDecalEffect();
		std::shared_ptr<game_module::GameObject> EmitJumpDecalEndEffect();

		/// <summary>
		/// �޺� ���� ������ ���� 
		/// </summary>
		void EmitComboAttack(float xAxisOffset);

		/// <summary>
		/// ������� ���� ������ ����
		/// </summary>
		void EmitSmashDown();

		/// <summary>
		/// ���� ���� ������ ����
		/// </summary>
		void EmitJumpAttack();

		/// <summary>
		/// ���� ���� ������ ����
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitRushAttack();

#pragma endregion EmitEffect/Attack

#pragma region Getter/Setter
		/// <summary>
		/// Ÿ���� �����մϴ� 
		/// </summary>
		void SetTarget(game_module::GameObject* target) override;

		/// <summary>
		/// ������ �ܰ��� ������ �����մϴ�
		/// </summary>
		void SetOutLineColor(DirectX::SimpleMath::Color color);

		/// <summary>
		/// ������ ������Ʈ �÷��� �����մϴ� 
		/// </summary>
		void SetRimLightColor(bool bUseRimLight, DirectX::SimpleMath::Color color, float rimPow = 1.f) const;
		void SetInvRimLightColor(bool bUseInvRimLight, DirectX::SimpleMath::Color color, float invRimPow = 1.f) const;

		/// <summary>
		/// ������ ������ ��ġ�� �����մϴ�.
		/// </summary>
		void SetJumpPosition(const DirectX::SimpleMath::Vector3& jumpPosition);

		/// <summary>
		/// ��� ���¿� �� ī��Ʈ�� �����մϴ�.
		/// </summary>
		void GenerateAngryEnteringVigilantCount();
		void GenerateEnteringVigilantCount();

		KnockBack* GetKnockBack() const { return mKnockBack; }
		std::shared_ptr<game_module::GameObject> GetTarget() override { return mTarget; };
		const DirectX::SimpleMath::Vector3& GetJumpPosition() const;
		const DirectX::SimpleMath::Vector3& GetDefaultJumpPosition() const;
		float GetHPRatio() const;
		float GetGroggyGaugeRatio()const;
		float GetAttackPower() const;
		float GetAngryPreAttackSpeedRatio() const;
		float GetAngryAttackSpeedRatio() const;
		float GetAttackCoolTime() const;
		unsigned int GetAngryEnteringVigilantCount() const;
		unsigned int GetEnteringVigilantCount() const;

#pragma endregion Getter/Setter

	private:
		void waitAttack();
		EBossMonsterAttackType getNextPattern(bool bIsInludeEat)const;
		void setNextPattern();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;

		void initGroggy();
		void playBossBGM();

		void updateGroggy(float dt);
		void updateJump(float dt);

		void destroySocketCollider();

		bool isGroggyState() const;
		bool canJump(const game_module::Transform* transform) const;
		bool canRoar() const;

		void processAttack(class Attack* attack);

		void setDecalEffectSpeed(game_module::GameObject* gameObject, float speed);

	private:
		// ����
		GameManager* mGameManager;
		KnockBack* mKnockBack;
		fq::game_module::Transform* mTransform;
		fq::game_module::Animator* mAnimator;
		fq::game_module::SkinnedMeshRenderer* mSkinnedMesh;
		std::shared_ptr<game_module::GameObject> mTarget;
		std::shared_ptr<game_module::GameObject> mHpBar;

		bool mbUseBossBGM;
		std::string mBossBGMKey;

		// ���� �ɷ�ġ
		float mMaxHp;
		float mHp;
		float mAttackPower;
		float mMoveSpeed;
		float mAcceleration;
		float mRotationSpeed;

		// ���� ����
		float mAttackRange;
		float mDetectRange;

		// ���� �˹�
		float mRushKnockBackPower;
		float mSmashKnockBackPower;
		float mComboAttackKnockBackPower;
		float mContinousKnockBackPower;
		float mJumpAttackKnockBack;

		// ��Ÿ��
		float mAttackElapsedTime;
		float mAttackCoolTime;
		float mMinWaitAttackTime;
		float mMaxWaitAttackTime;

		// ���� ���� ����
		float mSmashDownOffset;
		float mComboAttackOffset;
		float mSecondComboAttackRatio;
		float mComboAttackReboundPower;
		float mContinousAttackReboundPower;
		float mEffectYOffset; // ������ ���� �ִ� ����Ʈ�� Y�� ����

		// �׷α� ���� 
		float mStartGroggyGauge; // �׷α� ���� ������
		float mGroggyGauge; // ���� �׷α� ������
		float mGroggyIncreaseRatio; // �׷α� ��� ����
		float mGroggyDecreasePerSecond; // �׷α� ���°� �ƴ� �� ���� ����
		float mGroggyDecreaseHpRatio; // �׷α� ���� hp ���Һ���
		float mGroggyDuration; // �׷α� ���� �ð�
		float mGroggyElapsed;

		// ���� Ȯ�� ���� 
		float mRushProbability;
		float mSmashProbability;
		float mRoarProbability;
		float mContinousProbability;
		float mEatProbability;

		// ���� ������
		fq::game_module::PrefabResource mSmashDownAttack;
		fq::game_module::PrefabResource mComboAttack;
		fq::game_module::PrefabResource mContinousAttack; // �̳� �߰�
		fq::game_module::PrefabResource mRushAttack;
		fq::game_module::PrefabResource mJumpDownAttack;

		// ����Ʈ ������
		fq::game_module::PrefabResource mSmashDownEffect;
		fq::game_module::PrefabResource mSmashDownDecalEffect;
		fq::game_module::PrefabResource mComboEffect;
		fq::game_module::PrefabResource mComboDecalEffect;
		fq::game_module::PrefabResource mRushEffect;
		fq::game_module::PrefabResource mRushDecalEffect;
		fq::game_module::PrefabResource mJumpEffect;
		fq::game_module::PrefabResource mJumpDecalEffect;
		fq::game_module::PrefabResource mJumpDecalEndEffect;

		// ü�¹� ������
		fq::game_module::PrefabResource mHpBarPrefab;

		// todo : ���� �÷��̾� ���� ��� ���ȭ �ϱ�
		// ���� �÷��̾� ���� ���� ���� 
		float mDummyTraceDurationTime;
		bool mbUseDummyTraceRandomRange;
		float mDummyDurationRandomRangeMin;
		float mDummyDurationRandomRangeMax;
		float mCurrentDummyTraceDurationTime;
		float mDummyTraceElapsedTime;
		bool mIsDummyTarget;

		// ���� ����
		float mJumpDistance;
		float mJumpAttackPower;
		DirectX::SimpleMath::Vector3 mJumpPosition;
		DirectX::SimpleMath::Vector3 mDefaultJumpPosition;
		float mJumpCoolTime;
		float mJumpElapsed;

		// ȭ�� ���� ó��
		float mArrowImotalTime;
		float mArrowHitDuration;

		// 2������ ����
		bool mbEnterAngryState;
		float mAngryRatio;
		float mAngrySpeedRatio;
		float mAngryCoolTimeRatio;
		float mAngryPreAttackSpeedRatio;
		float mAngryAttackSpeedRatio;
		float mAngryAttackPowerRatio;
		unsigned int mAngryEnteringVigilantCount;
		unsigned int mAngryEnteringVigilantMinCount;
		unsigned int mAngryEnteringVigilantMaxCount;
		unsigned int mVigilantCount;
		unsigned int mVigilantMinCount;
		unsigned int mVigilantMaxCount;
		float mAngryGroggyDecreasePerSecond;
		float mAngryGroggyDuration;

		//Roar ���� ����
		unsigned int mMinMonsterCount;
		float mRoarCoolTime;
		float mRoarElapsed;

		// ĳ���� �� ����� ����
		float mKnightDamageRatio;
		float mMagicDamageRatio;
		float mArcherDamageRatio;
		float mBerserkerDamageRatio;

		// ��Į �ӵ�
		float mSmashDownDecalEffectSpeed;
		float mComboDecalEffectSpeed;
		float mRushDecalEffectSpeed;
		float mJumpDecalEffectSpeed;

		// ���� �ο��� �� ó��
		float mPlayer1HP;
		float mPlayer2HP;
		float mPlayer3HP;
		float mPlayer4HP;

		float mPlayer1GroggyIncreaseRatio;
		float mPlayer2GroggyIncreaseRatio;
		float mPlayer3GroggyIncreaseRatio;
		float mPlayer4GroggyIncreaseRatio;

		friend void RegisterMetaData();
	};
}