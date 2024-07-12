#pragma once

#include "../FQGameModule/GameModule.h"
#include "IMonster.h"

namespace fq::client
{
	class GameManager;

	/// <summary>
	/// ��������
	/// </summary>
	class MeleeMonster : public game_module::Component , public IMonster
	{
	public:
		MeleeMonster();
		~MeleeMonster();

		void SetTarget(game_module::GameObject* target) override;
		void EmitAttack();
		void ChaseTarget();

		/// <summary>
		/// Ÿ���� ���� ������ �ִ��� Ȯ���մϴ�
		/// </summary>
		void DetectTarget();

		/// <summary>
		/// Ÿ���� �����Ǿ� �ٸ� �׷쿡�� �˸��ϴ�
		/// </summary>
		void AnnounceFindedTarget();

		/// <summary>
		/// Ÿ���� ���ݹ����� �ִ��� Ȯ���մϴ�
		/// </summary>
		void CheckTargetInAttackRange();

		void Move(DirectX::SimpleMath::Vector3 destination);
		void Patrol();

		void CheckAttackAble()const;
	
	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;


	private:
		GameManager* mGameManager;
		fq::game_module::Transform* mTransform;
		fq::game_module::Animator* mAnimator;
		std::shared_ptr<game_module::GameObject> mTarget;

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

		DirectX::SimpleMath::Vector3 mStartPosition;
		DirectX::SimpleMath::Vector3 mPatrolDestination;

		friend void RegisterMetaData();
	};


}
