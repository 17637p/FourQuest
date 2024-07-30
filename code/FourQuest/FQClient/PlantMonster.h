#pragma once

#include "../FQGameModule/GameModule.h"
#include "IMonster.h"

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

		void SetTarget(fq::game_module::GameObject* target) override;

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
		fq::game_module::PrefabResource mAttackPrefab;

		friend void RegisterMetaData();
	};


}