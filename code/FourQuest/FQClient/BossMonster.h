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
	/// 웅장한 근접 보스몬스터 클래스
	/// 
	/// </summary>
	class BossMonster : public game_module::Component
	{
	public:
		BossMonster();
		~BossMonster();

		/// <summary>
		/// 타겟이 감지 범위에 있는지 확인합니다 
		/// </summary>
		void DetectTarget();

		/// <summary>
		/// 타겟을 설정합니다 
		/// </summary>
		void SetTarget(game_module::GameObject* target);

		/// <summary>
		/// 타겟을 추적합니다 
		/// </summary>
		void ChaseTarget();

		/// <summary>
		/// 타겟이 공격범위에 있는지 확인합니다
		/// </summary>
		void CheckTargetInAttackRange();

		/// <summary>
		/// 내려찍기 공격 
		/// </summary>
		void SmashDown();

		/// <summary>
		/// 내려찍기 공격 이펙트를 방출합니다 
		/// </summary>
		std::shared_ptr<game_module::GameObject> EmitSmashDownEffect();

		void Move(DirectX::SimpleMath::Vector3 destination);

		/// <summary>
		/// 돌진합니다 
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