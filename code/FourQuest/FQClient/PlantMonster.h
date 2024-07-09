#pragma once

#include "../FQGameModule/GameModule.h"
#include "IMonster.h"

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

		void SetTarget(fq::game_module::GameObject* target) override;

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
		fq::game_module::PrefabResource mAttackPrefab;

		friend void RegisterMetaData();
	};


}