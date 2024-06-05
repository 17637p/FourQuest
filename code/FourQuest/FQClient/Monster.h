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

		void OnStart() override;
		void OnUpdate(float dt) override;

		void ChangeState(const std::string& nextState);

		// State ��
		float CalculateDistance(fq::game_module::GameObject gameObject);

		game_module::Animator* GetAnimator() const;
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

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		MonsterFSM* mFSM;
		std::string mCurState;

		game_module::Animator* mAnimator;
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
	};
}

