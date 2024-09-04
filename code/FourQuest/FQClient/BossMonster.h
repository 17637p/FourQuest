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
	class GameManager;

	/// <summary>
	/// ������ ���� �������� Ŭ����
	/// 
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
		/// �������� Ÿ���� �����մϴ� 
		/// </summary>
		void SetRandomTarget();

		/// <summary>
		/// ���� ������ ������ �����մϴ� 
		/// </summary>
		void SetNextAttack();

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
		void EmitComboAttack();

		/// <summary>
		/// �޺� ���� ������ �з����� �ݵ�
		/// </summary>
		void ReboundComboAttack();

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

		/// <summary>
		/// HPBar�� �����մϴ�
		/// </summary>
		void CreateHpBar();

		/// <summary>
		/// HpBar�� �����մϴ�
		/// </summary>
		void DestroryHpBar();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;

	private:
		GameManager* mGameManager;
		KnockBack* mKnockBack;
		fq::game_module::Transform* mTransform;
		fq::game_module::Animator* mAnimator;
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