#pragma once

#include "../FQGameModule/GameModule.h"
#include "IMonster.h"

namespace fq::client
{
	class GameManager;

	/// <summary>
	/// 근접몬스터
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
