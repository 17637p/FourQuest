#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "IMonster.h"

namespace fq::game_module
{
	class Animator;
}

namespace fq::client
{
	class GameManager;

	/// <summary>
	/// 원거리 몬스터 
	/// </summary>
	class PlantMonster :public game_module::Component , public IMonster
	{
	public:
		PlantMonster();
		~PlantMonster();

		/// <summary>
		/// 원거리 발사체 생성 
		/// </summary>
		void EmitAttack();

		/// <summary>
		/// 원거리 범위 공격 발사체 생성
		/// </summary>
		void EmitAOEAttack();

		void SetTarget(fq::game_module::GameObject* target) override;
		std::shared_ptr<game_module::GameObject> GetTarget() override { return mTarget; };

		/// <summary>
		/// 타겟을 응시합니다 
		/// </summary>
		void LookAtTarget();

		/// <summary>
		/// 타겟이 공격범위에 있는지 확인합니다
		/// </summary>
		void CheckTargetInAttackRange();

		/// <summary>
		/// 공격 가능한 상태인지 확인합니다
		/// </summary>
		void CheckAttackAble();

		/// <summary>
		/// 타겟을 찾습니다
		/// </summary>
		void FindTarget();
	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;

	private:
		GameManager* mGameManager;

		game_module::Animator* mAnimator;
		std::shared_ptr<game_module::GameObject> mTarget;
		float mAttackElapsedTime;
		
		/* Plane Moster 설정 관련 */
		float mMaxHp;
		float mHp;
		float mAttackPower; 
		float mProjectileSpeed;
		float mAttackRange;
		float mAttackCoolTime;
		float mRotationSpeed; 
		bool mbIsAOEAttacker;
		fq::game_module::PrefabResource mAttackPrefab;
		fq::game_module::PrefabResource mAOEAttackPrefab;

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