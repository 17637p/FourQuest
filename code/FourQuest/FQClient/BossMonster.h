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
		~BossMonster();

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
		void SetTarget(game_module::GameObject* target) override;
		std::shared_ptr<game_module::GameObject> GetTarget() override { return mTarget; };

		/// <summary>
		/// Ÿ���� �����մϴ� 
		/// </summary>
		void ChaseTarget();

		/// <summary>
		/// �÷��̾ ��ġ�� �°� 
		/// </summary>
		void HomingTarget();

		/// <summary>
		/// Ÿ���� ���ݹ����� �ִ��� Ȯ���մϴ�
		/// </summary>
		void CheckTargetInAttackRange();

		/// <summary>
		/// ������� ���� 
		/// </summary>
		void EmitSmashDown();

		/// <summary>
		/// �޺� ���� 
		/// </summary>
		void EmitComboAttack(float xAxisOffset);

		/// <summary>
		/// �޺�, ���Ӱ��� �� ������ �з����� �ݵ�
		/// </summary>
		void ReboundComboAttack();

		/// <summary>
		/// ���� ���� ���� ������ �ڷ� ��¦ �������� 
		/// </summary>
		void StepBack();

		/// <summary>
		/// ������� ���� ����Ʈ�� �����մϴ� 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitSmashDownEffect();

		void Move(DirectX::SimpleMath::Vector3 destination);

		/// <summary>
		/// �����մϴ� 
		/// </summary>
		std::shared_ptr<game_module::GameObject> Rush();

		float GetHPRatio()const;

		float GetGroggyGaugeRatio()const;

		void AddHp(float hp);
	
		/// <summary>
		/// HPBar�� �����մϴ�
		/// </summary>
		void CreateHpBar();

		/// <summary>
		/// ���� �� �ִ� ���� ����� ������ Ž���ϰ� Ÿ������ �����մϴ�
		/// </summary>
		bool FindAndSetTargetDeadArmour();

		/// <summary>
		/// HpBar�� �����մϴ�
		/// </summary>
		void DestroryHpBar();

		/// <summary>
		/// Ÿ���� �������� �����մϴ� 
		/// </summary>
		void SetRandomTarget();

		/// <summary>
		/// ������ ������Ʈ �÷��� �����մϴ� 
		/// </summary>
		void SetRimLightColor(bool bUseRimLight ,DirectX::SimpleMath::Color color)const;

		/// <summary>
		/// ������ �ܰ��� ������ �����մϴ�
		/// </summary>
		void SetOutLineColor(DirectX::SimpleMath::Color color);

	private:
		void waitAttack();
		EBossMonsterAttackType getNextPattern(bool bIsInludeEat)const;
		void setNextPattern();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;

		bool isGroggyState()const;

	private:
		GameManager* mGameManager;
		KnockBack* mKnockBack;
		fq::game_module::Transform* mTransform;
		fq::game_module::Animator* mAnimator;
		fq::game_module::SkinnedMeshRenderer* mSkinnedMesh;
		std::shared_ptr<game_module::GameObject> mTarget;

		float mMaxHp;
		float mHp;
		float mAttackPower;
		float mMoveSpeed;
		float mAcceleration;
		float mAttackRange;
		float mAttackCoolTime;
		float mAttackElapsedTime;	
		float mSmashDownOffset;
		float mComboAttackOffset;
		float mRushPower;
		float mComboAttackReboundPower;
		float mDetectRange;
		float mRotationSpeed;
		float mSecondComboAttackRatio;
		float mMinWaitAttackTime;
		float mMaxWaitAttackTime;
		float mRushKnockBackPower;
		float mSmashKnockBackPower;
		float mComboAttackKnockBackPower;
		float mContinousKnockBackPower;

		// �׷α� ���� 
		float mStartGroggyGauge;
		float mGroggyGauge;
		float mGroggyIncreaseRatio;
		float mGroggyDecreasePerSecond;

		// ���� Ȯ�� ���� 
		float mRushProbability;
		float mSmashProbability;
		float mRoarProbability;
		float mContinousProbability;
		float mEatProbability;

		fq::game_module::PrefabResource mSmashDownAttack;
		fq::game_module::PrefabResource mSmashDownEffect;
		fq::game_module::PrefabResource mComboAttack;
		fq::game_module::PrefabResource mComboEffect;
		fq::game_module::PrefabResource mRushAttack;
		fq::game_module::PrefabResource mRushEffect;

		std::shared_ptr<game_module::GameObject> mHpBar;
		fq::game_module::PrefabResource mHpBarPrefab;

		friend void RegisterMetaData();
	};
}