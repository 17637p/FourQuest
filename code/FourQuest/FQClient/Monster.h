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

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnTriggerEnter(const fq::game_module::Collision& collision) override;

		// State ��
		float CalculateDistanceTarget(fq::game_module::GameObject* gameObject);

		std::shared_ptr<fq::game_module::GameObject> GetTarget() const;
		void SetTarget(fq::game_module::GameObject* target);
		bool GetIsDamaged() const;
		void SetIsDamaged(bool isDamaged);
		float GetDamaged() const;
		float GetLastAttackTime() const { return mLastAttackTime; }
		std::shared_ptr<fq::game_module::GameObject> GetLastAttacker() const;

		// ���� ������ ����
		float GetMaxHP() const { return mMaxHP; }

		float GetHP() const;
		void SetHP(float hp);

		float GetAttackPower() const;
		void SetAttackPower(float attackPower);

		float GetMoveSpeed() const;
		void SetMoveSpeed(float moveSpeed);

		float GetAcceleration() const { return mAcceleration; }
		void SetAcceleration(float val) { mAcceleration = val; }

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
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		std::shared_ptr<fq::game_module::GameObject> mTarget;
		//fq::game_module::GameObject* mTarget;

		bool mIsDamaged;
		float mDamaged; // ���� ���� ������ �� 
		std::shared_ptr<fq::game_module::GameObject> mLastAttacker;
		//fq::game_module::GameObject* mLastAttacker; // ���������� ������ �÷��̾�
		float mLastAttackTime;

		// Stat
		float mMaxHP;
		float mHP;
		float mAttackPower;
		float mMoveSpeed;
		float mAcceleration;

		float mTargetAttackRange;
		float mChaseDistance;
		float mAttackWaitTime;
		float mDamagedDelay;

		fq::game_module::PrefabResource mAttack;
	};
}

