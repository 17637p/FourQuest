#pragma once

#include "../FQGameModule/GameModuleEnum.h"
#include "../FQGameModule/GameModule.h"

#include "AttackDefine.h"

namespace fq::client
{
	class ArrowAttack : public game_module::Component
	{
	public:
		ArrowAttack();
		~ArrowAttack();

		/// <summary>
		/// ȭ�� ������Ʈ�� ��� ������Ʈ�� ������ �Լ�
		/// </summary>
		virtual void OnTriggerEnter(const game_module::Collision& collision) override;

		/// <summary>
		/// ȭ�� ���� ����
		/// </summary>
		void Set(const ArrowAttackInfo& info);

		/// <summary>
		/// ȭ�� �ǰ� ���� ���
		/// </summary>
		void PlayHitSound();

		bool GetIsStrongAttack() const { return mbIsStrongAttack; }
		void SetIsStrongAttack(bool isStrongAttack) { mbIsStrongAttack = isStrongAttack; }
		float GetWeakAttackPower() const { return mWeakAttackPower; }
		void SetWeakAttackPower(float attackPower) { mWeakAttackPower = attackPower; }
		float GetStrongAttackPower() const { return mStrongAttackPower; }
		void SetStrongAttackPower(float attackPower) { mStrongAttackPower = attackPower; }
		int GetIsBlock() const { return mMaxBlockCount; }
		void SetIsBlock(int maxBlockCount) { mMaxBlockCount = maxBlockCount; }
		float GetLifeTime() const { return mLifeTime; }
		void SetLifeTime(float time) { mLifeTime = time; }

	private:
		/// <summary>
		/// ȭ���� ������ Ÿ���� ����ϰ� �ð��� ������ ȭ�� ������Ʈ ����
		/// </summary>
		void OnUpdate(float dt) override;

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		// ������, ����� ���ݷ� �� �ִ� ���� ����
		bool  mbIsStrongAttack;
		float mWeakAttackPower;
		float mStrongAttackPower;
		float mWeakProjectileVelocity;
		float mStrongProjectileVelocity;
		DirectX::SimpleMath::Vector3 mAttackDirection;
		DirectX::SimpleMath::Matrix  mAttackTransform;
		int mMaxBlockCount;

		// ���� �ð�
		float mLifeElapsedTime;
		float mLifeTime;

		// ������ �ϴ� ��ü 
		fq::game_module::GameObject* mAttacker;
		std::string mHitSound;
	};
}