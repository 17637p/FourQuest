#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "IMonster.h"

namespace fq::game_module
{
	class Transform;
	class Animator;
}

namespace fq::client
{
	class KnockBack;
	class MonsterHP;
	class GameManager;

	/// <summary>
	/// 근접몬스터
	/// </summary>
	class MeleeMonster : public game_module::Component, public IMonster
	{
	public:
		MeleeMonster();
		~MeleeMonster();

		void SetTarget(game_module::GameObject* target) override;
		std::shared_ptr<game_module::GameObject> GetTarget() override { return mTarget; };

		void EmitAttack();
		std::shared_ptr<game_module::GameObject> EmitAttackEffect();
		void ChaseTarget();

		/// <summary>
		/// 타겟이 감지 범위에 있는지 확인합니다
		/// </summary>
		void DetectTarget();

		/// <summary>
		/// 타겟이 감지되어 다른 그룹에게 알립니다	
		/// </summary>
		void AnnounceFindedTarget();

		/// <summary>
		/// 타겟이 공격범위에 있는지 확인합니다
		/// </summary>
		void CheckTargetInAttackRange();

		void Move(DirectX::SimpleMath::Vector3 destination);
		void Patrol();

		void CheckAttackAble()const;

		/// <summary>
		/// 타겟을 응시합니다
		/// </summary>
		void LookAtTarget();

		void DestroyMonsterHPUI();

		void HitArrow(fq::game_module::GameObject* object);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;

		void destroySocketCollider();

	private:
		GameManager* mGameManager;
		fq::game_module::Transform* mTransform;
		fq::game_module::Animator* mAnimator;
		MonsterHP* mMonsterHpUI;
		KnockBack* mKnockBack;
		std::shared_ptr<game_module::GameObject> mTarget;
		float mArrowImotalTime;
		float mArrowHitDuration;

		float mMaxHp;
		float mHp;
		float mAttackPower;
		float mMoveSpeed;
		float mAcceleration;
		float mAttackRange;
		float mPatrolRange;
		float mDetectRange;
		float mAttackCoolTime;
		float mAttackElapsedTime;
		float mAttackOffset;

		fq::game_module::PrefabResource mAttackPrefab;
		fq::game_module::PrefabResource mAttackEffect;

		DirectX::SimpleMath::Vector3 mStartPosition;
		DirectX::SimpleMath::Vector3 mPatrolDestination;

		// 더미 플레이어 추적 관련 변수
		float mDummyTraceDurationTime;
		bool mbUseDummyTraceRandomRange;
		float mDummyDurationRandomRangeMin;
		float mDummyDurationRandomRangeMax;
		float mCurrentDummyTraceDurationTime; // traceDuration + randomRange;
		float mDummyTraceElapsedTime;
		bool mIsDummyTarget;

		friend void RegisterMetaData();
	};


}
