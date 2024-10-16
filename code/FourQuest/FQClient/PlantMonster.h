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
	/// ���Ÿ� ���� 
	/// </summary>
	class PlantMonster :public game_module::Component , public IMonster
	{
	public:
		PlantMonster();
		~PlantMonster();

		/// <summary>
		/// ���Ÿ� �߻�ü ���� 
		/// </summary>
		void EmitAttack();

		/// <summary>
		/// ���Ÿ� ���� ���� �߻�ü ����
		/// </summary>
		void EmitAOEAttack();

		void SetTarget(fq::game_module::GameObject* target) override;
		std::shared_ptr<game_module::GameObject> GetTarget() override { return mTarget; };

		/// <summary>
		/// Ÿ���� �����մϴ� 
		/// </summary>
		void LookAtTarget();

		/// <summary>
		/// Ÿ���� ���ݹ����� �ִ��� Ȯ���մϴ�
		/// </summary>
		void CheckTargetInAttackRange();

		/// <summary>
		/// ���� ������ �������� Ȯ���մϴ�
		/// </summary>
		void CheckAttackAble();

		/// <summary>
		/// Ÿ���� ã���ϴ�
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
		
		/* Plane Moster ���� ���� */
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

		// ���� �÷��̾� ���� ���� ����
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