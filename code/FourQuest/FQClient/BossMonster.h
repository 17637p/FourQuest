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
		~BossMonster() = default;

		/// <summary>
		/// 타겟이 감지 범위에 있는지 확인합니다 
		/// </summary>
		void DetectTarget();

		/// <summary>
		/// 보스 몬스터 패턴을 종료한 후에 호출합니다.
		/// </summary>
		void EndPattern();

		/// <summary>
		/// 타겟을 추적합니다 
		/// </summary>
		void ChaseTarget();

		/// <summary>
		/// 플레이어를 위치에 맞게 
		/// </summary>
		void HomingTarget(bool bUseSpeed = true);

		/// <summary>
		/// 타겟이 공격범위에 있는지 확인합니다
		/// </summary>
		void CheckTargetInAttackRange();

		/// <summary>
		/// 먹을 수 있는 가장 가까운 갑옷을 탐색하고 타겟으로 지정합니다
		/// </summary>
		bool FindAndSetTargetDeadArmour();

		/// <summary>
		/// 타겟을 랜덤으로 설정합니다 
		/// </summary>
		void SetRandomTarget();

		void Move(DirectX::SimpleMath::Vector3 destination);

		/// <summary>
		/// 콤보, 연속공격 시 앞으로 밀려나는 반동
		/// </summary>
		void ReboundComboAttack();

		/// <summary>
		/// 연속 공격 시전 이전에 뒤로 살짝 물러나기 
		/// </summary>
		void StepBack();

		void HitArrow(fq::game_module::GameObject* object);

		bool IsAngry() const;

		void ResetJumpCoolTime();
		void ResetRourCoolTime();

#pragma region HPBar
		void AddHp(float hp);

		/// <summary>
		/// HPBar를 생성합니다
		/// </summary>
		void CreateHpBar();

		/// <summary>
		/// HpBar를 제거합니다
		/// </summary>
		void DestroryHpBar();
#pragma endregion HPBar

#pragma region EmitEffect/Attack
		/// <summary>
		/// 내려찍기 공격 이펙트를 방출합니다 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitSmashDownEffect();

		/// <summary>
		/// 내려찍기 공격 데칼 이펙트를 방출합니다
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitSmashDecalEffect();

		void SetSmashDecalEffect(std::shared_ptr<game_module::GameObject> gameObject);

		/// <summary>
		/// 콤보 공격 이펙트를 방출합니다 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitComboEffect(float xAxisOffset);

		/// <summary>
		/// 콤보 공격 데칼 이펙트를 방출합니다 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitComboDecalEffect(float xAxisOffset);

		void SetComboDecalEffect(std::shared_ptr<game_module::GameObject> gameObject, float xAxisOffset);

		/// <summary>
		/// 대쉬 공격 이펙트를 방출합니다
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitRushEffect();

		/// <summary>
		/// 대쉬 공격 데칼 이펙트를 방출합니다
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitRushDecalEffect();

		/// <summary>
		/// 점프 공격 이펙트를 방출합니다
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitJumpEffect();

		/// <summary>
		/// 점프 공격의 데칼 이펙트를 방출합니다
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitJumpDecalEffect();
		std::shared_ptr<game_module::GameObject> EmitJumpDecalEndEffect();

		/// <summary>
		/// 콤보 공격 프리팹 생성 
		/// </summary>
		void EmitComboAttack(float xAxisOffset);

		/// <summary>
		/// 내려찍기 공격 프리팹 생성
		/// </summary>
		void EmitSmashDown();

		/// <summary>
		/// 점프 공격 프리팹 생성
		/// </summary>
		void EmitJumpAttack();

		/// <summary>
		/// 러쉬 공격 프리팹 생성
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitRushAttack();

#pragma endregion EmitEffect/Attack

#pragma region Getter/Setter
		/// <summary>
		/// 타겟을 설정합니다 
		/// </summary>
		void SetTarget(game_module::GameObject* target) override;

		/// <summary>
		/// 보스의 외각선 색깔을 설정합니다
		/// </summary>
		void SetOutLineColor(DirectX::SimpleMath::Color color);

		/// <summary>
		/// 보스의 림라이트 컬러를 설정합니다 
		/// </summary>
		void SetRimLightColor(bool bUseRimLight, DirectX::SimpleMath::Color color, float rimPow = 1.f) const;
		void SetInvRimLightColor(bool bUseInvRimLight, DirectX::SimpleMath::Color color, float invRimPow = 1.f) const;

		/// <summary>
		/// 보스가 점프뛸 위치를 설정합니다.
		/// </summary>
		void SetJumpPosition(const DirectX::SimpleMath::Vector3& jumpPosition);

		/// <summary>
		/// 경계 상태에 들어갈 카운트를 세팅합니다.
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
		// 참조
		GameManager* mGameManager;
		KnockBack* mKnockBack;
		fq::game_module::Transform* mTransform;
		fq::game_module::Animator* mAnimator;
		fq::game_module::SkinnedMeshRenderer* mSkinnedMesh;
		std::shared_ptr<game_module::GameObject> mTarget;
		std::shared_ptr<game_module::GameObject> mHpBar;

		bool mbUseBossBGM;
		std::string mBossBGMKey;

		// 보스 능력치
		float mMaxHp;
		float mHp;
		float mAttackPower;
		float mMoveSpeed;
		float mAcceleration;
		float mRotationSpeed;

		// 감지 범위
		float mAttackRange;
		float mDetectRange;

		// 공격 넉백
		float mRushKnockBackPower;
		float mSmashKnockBackPower;
		float mComboAttackKnockBackPower;
		float mContinousKnockBackPower;
		float mJumpAttackKnockBack;

		// 쿨타임
		float mAttackElapsedTime;
		float mAttackCoolTime;
		float mMinWaitAttackTime;
		float mMaxWaitAttackTime;

		// 보스 공격 조정
		float mSmashDownOffset;
		float mComboAttackOffset;
		float mSecondComboAttackRatio;
		float mComboAttackReboundPower;
		float mContinousAttackReboundPower;
		float mEffectYOffset; // 가려질 위험 있는 이펙트에 Y축 보정

		// 그로기 관련 
		float mStartGroggyGauge; // 그로기 시작 게이지
		float mGroggyGauge; // 현재 그로기 게이지
		float mGroggyIncreaseRatio; // 그로기 상승 비율
		float mGroggyDecreasePerSecond; // 그로기 상태가 아닐 때 감소 비율
		float mGroggyDecreaseHpRatio; // 그로기 상태 hp 감소비율
		float mGroggyDuration; // 그로기 지속 시간
		float mGroggyElapsed;

		// 패턴 확률 관련 
		float mRushProbability;
		float mSmashProbability;
		float mRoarProbability;
		float mContinousProbability;
		float mEatProbability;

		// 공격 프리팹
		fq::game_module::PrefabResource mSmashDownAttack;
		fq::game_module::PrefabResource mComboAttack;
		fq::game_module::PrefabResource mContinousAttack; // 이넘 추가
		fq::game_module::PrefabResource mRushAttack;
		fq::game_module::PrefabResource mJumpDownAttack;

		// 이펙트 프리팹
		fq::game_module::PrefabResource mSmashDownEffect;
		fq::game_module::PrefabResource mSmashDownDecalEffect;
		fq::game_module::PrefabResource mComboEffect;
		fq::game_module::PrefabResource mComboDecalEffect;
		fq::game_module::PrefabResource mRushEffect;
		fq::game_module::PrefabResource mRushDecalEffect;
		fq::game_module::PrefabResource mJumpEffect;
		fq::game_module::PrefabResource mJumpDecalEffect;
		fq::game_module::PrefabResource mJumpDecalEndEffect;

		// 체력바 프리팹
		fq::game_module::PrefabResource mHpBarPrefab;

		// todo : 더미 플레이어 추적 기능 모듈화 하기
		// 더미 플레이어 추적 관련 변수 
		float mDummyTraceDurationTime;
		bool mbUseDummyTraceRandomRange;
		float mDummyDurationRandomRangeMin;
		float mDummyDurationRandomRangeMax;
		float mCurrentDummyTraceDurationTime;
		float mDummyTraceElapsedTime;
		bool mIsDummyTarget;

		// 점프 공격
		float mJumpDistance;
		float mJumpAttackPower;
		DirectX::SimpleMath::Vector3 mJumpPosition;
		DirectX::SimpleMath::Vector3 mDefaultJumpPosition;
		float mJumpCoolTime;
		float mJumpElapsed;

		// 화살 박힘 처리
		float mArrowImotalTime;
		float mArrowHitDuration;

		// 2페이즈 관련
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

		//Roar 패턴 관련
		unsigned int mMinMonsterCount;
		float mRoarCoolTime;
		float mRoarElapsed;

		// 캐릭터 별 대미지 배율
		float mKnightDamageRatio;
		float mMagicDamageRatio;
		float mArcherDamageRatio;
		float mBerserkerDamageRatio;

		// 데칼 속도
		float mSmashDownDecalEffectSpeed;
		float mComboDecalEffectSpeed;
		float mRushDecalEffectSpeed;
		float mJumpDecalEffectSpeed;

		// 몬스터 인원수 별 처리
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