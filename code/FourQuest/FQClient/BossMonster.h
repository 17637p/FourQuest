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
	/// 웅장한 근접 보스몬스터 클래스
	/// </summary>
	class BossMonster : public game_module::Component, public IMonster
	{
	public:
		BossMonster();
		~BossMonster();

		/// <summary>
		/// 타겟이 감지 범위에 있는지 확인합니다 
		/// </summary>
		void DetectTarget();

		/// <summary>
		/// 보스 몬스터 패턴을 종료한 후에 호출합니다.
		/// </summary>
		void EndPattern();

		/// <summary>
		/// 타겟을 설정합니다 
		/// </summary>
		void SetTarget(game_module::GameObject* target) override;
		std::shared_ptr<game_module::GameObject> GetTarget() override { return mTarget; };

		/// <summary>
		/// 타겟을 추적합니다 
		/// </summary>
		void ChaseTarget();

		/// <summary>
		/// 플레이어를 위치에 맞게 
		/// </summary>
		void HomingTarget();

		/// <summary>
		/// 타겟이 공격범위에 있는지 확인합니다
		/// </summary>
		void CheckTargetInAttackRange();

		/// <summary>
		/// 내려찍기 공격 
		/// </summary>
		void EmitSmashDown();

		/// <summary>
		/// 콤보 공격 
		/// </summary>
		void EmitComboAttack(float xAxisOffset);

		/// <summary>
		/// 콤보, 연속공격 시 앞으로 밀려나는 반동
		/// </summary>
		void ReboundComboAttack();

		/// <summary>
		/// 연속 공격 시전 이전에 뒤로 살짝 물러나기 
		/// </summary>
		void StepBack();

		/// <summary>
		/// 내려찍기 공격 이펙트를 방출합니다 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitSmashDownEffect();

		void Move(DirectX::SimpleMath::Vector3 destination);

		/// <summary>
		/// 돌진합니다 
		/// </summary>
		std::shared_ptr<game_module::GameObject> Rush();

		float GetHPRatio()const;

		float GetGroggyGaugeRatio()const;

		void AddHp(float hp);
	
		/// <summary>
		/// HPBar를 생성합니다
		/// </summary>
		void CreateHpBar();

		/// <summary>
		/// 먹을 수 있는 가장 가까운 값옷을 탐색하고 타겟으로 지정합니다
		/// </summary>
		bool FindAndSetTargetDeadArmour();

		/// <summary>
		/// HpBar를 제거합니다
		/// </summary>
		void DestroryHpBar();

		/// <summary>
		/// 타겟을 랜덤으로 설정합니다 
		/// </summary>
		void SetRandomTarget();

		/// <summary>
		/// 보스의 림라이트 컬러를 설정합니다 
		/// </summary>
		void SetRimLightColor(bool bUseRimLight ,DirectX::SimpleMath::Color color)const;

		/// <summary>
		/// 보스의 외각선 색깔을 설정합니다
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

		// 그로기 관련 
		float mStartGroggyGauge;
		float mGroggyGauge;
		float mGroggyIncreaseRatio;
		float mGroggyDecreasePerSecond;

		// 패턴 확률 관련 
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