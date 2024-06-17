#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class MonsterFSM;
	class fq::game_module::GameObject;

	class Monster : public fq::game_module::Component
	{
	public:
		Monster();
		~Monster();

		virtual void OnUpdate(float dt) override;
		virtual void OnTriggerEnter(const fq::game_module::Collision& collision) override;

		// State 용
		float CalculateDistanceTarget(fq::game_module::GameObject& gameObject);

		fq::game_module::GameObject* GetTarget() const;
		void SetTarget(fq::game_module::GameObject* target);
		bool GetIsDamaged() const;
		void SetIsDamaged(bool isDamaged);
		float GetDamaged() const;
		float GetLastAttackTime() const { return mLastAttackTime; }
		fq::game_module::GameObject* GetLastAttacker() const;

		// 수정 가능한 변수
		float GetHP() const;
		void SetHP(float hp);

		float GetAttackPower() const;
		void SetAttackPower(float attackPower);

		float GetMoveSpeed() const;
		void SetMoveSpeed(float moveSpeed);

		float GetChaseDistance() const;
		void SetChaseDistance(float chaseDistance);

		float GetAttackWaitTime() const;
		void SetAttackWaitTime(float attackWaitTime);

		float GetTargetAttackRange() const;
		void SetTargetAttackRange(float targetAttackRange);

		float GetDamagedDelay() const { return mDamagedDelay; }
		void SetDamagedDelay(float val) { mDamagedDelay = val; }

		fq::game_module::PrefabResource GetAttack() const { return mAttack; }
		void SetAttack(fq::game_module::PrefabResource val) { mAttack = val; }

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		fq::game_module::GameObject* mTarget;

		bool mIsDamaged;
		float mDamaged; // 실제 받은 데미지 값 
		fq::game_module::GameObject* mLastAttacker; // 마지막으로 공격한 플레이어
		float mLastAttackTime;

		// Stat
		float mHP;
		float mAttackPower;
		float mMoveSpeed;

		float mTargetAttackRange;
		float mChaseDistance;
		float mAttackWaitTime;
		float mDamagedDelay;

		fq::game_module::PrefabResource mAttack;
	};
}

