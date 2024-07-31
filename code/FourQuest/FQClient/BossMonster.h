#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

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
	class BossMonster : public game_module::Component
	{
	public:
		BossMonster();
		~BossMonster();

		/// <summary>
		/// Ÿ���� ���� ������ �ִ��� Ȯ���մϴ� 
		/// </summary>
		void DetectTarget();

		/// <summary>
		/// Ÿ���� �����մϴ� 
		/// </summary>
		void SetTarget(game_module::GameObject* target);

		/// <summary>
		/// Ÿ���� �����մϴ� 
		/// </summary>
		void ChaseTarget();

		/// <summary>
		/// Ÿ���� ���ݹ����� �ִ��� Ȯ���մϴ�
		/// </summary>
		void CheckTargetInAttackRange();

		/// <summary>
		/// ������� ���� 
		/// </summary>
		void SmashDown();

		/// <summary>
		/// ������� ���� ����Ʈ�� �����մϴ� 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitSmashDownEffect();

		void Move(DirectX::SimpleMath::Vector3 destination);

		/// <summary>
		/// �����մϴ� 
		/// </summary>
		void Rush();

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
		float mRushPower;
		float mDetectRange;

		fq::game_module::PrefabResource mSmashDownAttack;
		fq::game_module::PrefabResource mSmashDownEffect;
		fq::game_module::PrefabResource mRushAttack;
		fq::game_module::PrefabResource mRushEffect;



		friend void RegisterMetaData();
	};
}