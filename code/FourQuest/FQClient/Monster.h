#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class MonsterFSM;
	class fq::game_module::GameObject;

	class Monster : public fq::game_module::Component, 
		public std::enable_shared_from_this<Monster>
	{
	public:
		Monster();
		~Monster();

		// State ��
		float CalculateDistance(fq::game_module::GameObject gameObject);

		fq::game_module::GameObject* GetTarget() const;
		void SetTarget(fq::game_module::GameObject* target);
		bool GetIsDamagaed() const;
		float GetDamaged() const;
		fq::game_module::GameObject* GetLastAttacker() const;

		// ���� ������ ����
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

		fq::game_module::PrefabResource GetAttack() const { return mAttack; }
		void SetAttack(fq::game_module::PrefabResource val) { mAttack = val; }

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		fq::game_module::GameObject* mTarget;

		bool mIsDamaged;
		float mDamaged; // ���� ���� ������ �� 
		fq::game_module::GameObject* mLastAttacker; // ���������� ������ �÷��̾�

		// Stat
		float mHP;
		float mAttackPower;
		float mMoveSpeed;

		float mTargetAttackRange;
		float mChaseDistance;
		float mAttackWaitTime;

		fq::game_module::PrefabResource mAttack;
	};
}
