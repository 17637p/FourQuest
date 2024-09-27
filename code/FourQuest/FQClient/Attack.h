#pragma once
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/GameModuleEnum.h"

#include "AttackDefine.h"

namespace fq::client
{
	// ����Ϸ��� ���ݷ��� �����ϰ�
	// �ݶ��̴� ������ �����ϰ�
	// ���� �� ��ġ�� �����Ѵ��� 
	// Attack Object�� Tag�� �Է��ϸ� �˴ϴ�
	// Attacker �� Attack�� ��ȯ�ϴ� ������Ʈ�� �־��ּ���~
	class Attack : public fq::game_module::Component
	{
	public:
		Attack();
		~Attack();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		float GetAttackPower() const { return mAttackPower; }
		fq::game_module::GameObject* GetAttacker() const { return mAttacker; }
		DirectX::SimpleMath::Vector3 GetAttackDirection() const { return mAttackDirection; }
		DirectX::SimpleMath::Vector3 GetAttackPosition() const { return mAttackPosition; }

		/// <summary>
		/// ������ �о�� ���� ��ȯ�մϴ�
		/// </summary>
		float GetKnockBackPower() const { return mKnockBackPower; }

		/// <summary>
		/// ���ݿ� �˹��� �ִ����� ��ȯ�մϴ� 
		/// </summary>
		bool HasKnockBack()const;

		EKnockBackType GetKnockBackType()const { return mKnockBackType; }

		/// <summary>
		/// ���� ������ �����մϴ� 
		/// </summary>
		void Set(const AttackInfo& info);

		/// <summary>
		/// ����ī��Ʈ�� ����ϰ� ������ Ȱ��ȭ�Ǿ��ִ��� Ȯ���մϴ�
		/// </summary>
		bool ProcessAttack();

		/// <summary>
		/// �ǰ� ���带 ����մϴ� 
		/// </summary>
		void PlayHitSound();

		void SetDestroyTime(float destroyTime);

		float GetTargetPosRatio() const { return mTargetPosRatio; }

		float GetDirectionRatio() const { return mDirectionRatio; }

		void SetAttackPosition(const DirectX::SimpleMath::Vector3& attackPosition) { mAttackPosition = attackPosition; }

		void SetAttackDirection(DirectX::SimpleMath::Vector3 direction);

		const std::string& GetAttackEffectEvent() const { return mAttackEffectEvent; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnUpdate(float dt) override;

	private:
		// ���ݷ�
		float mAttackPower;
		float mDestroyTime;
		float mDestroyElapsedTime;

		// ����Ƚ���� �������� 
		bool mbIsInfinite;
		unsigned int mRemainingAttackCount;

		EKnockBackType mKnockBackType;
		DirectX::SimpleMath::Vector3 mAttackDirection; // ���� ����
		DirectX::SimpleMath::Vector3 mAttackPosition;
		// ���ݹ������� �о�� ��
		float mKnockBackPower;

		// ������ �ϴ� ��ü 
		fq::game_module::GameObject* mAttacker;
		std::string mHitSound;
		std::function<void()> mHitCallback;

		float mTargetPosRatio; // TargetPositionAndDirectionByAngle�� ����
		float mDirectionRatio; // TargetPositionAndDirectionByAngle�� ����

		std::string mAttackEffectEvent;

		friend void RegisterMetaData();
	};
}

