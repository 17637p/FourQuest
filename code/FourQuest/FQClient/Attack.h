#pragma once
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/GameModuleEnum.h"

#include "AttackDefine.h"

namespace fq::client
{
	// 사용하려면 공격력을 설정하고
	// 콜라이더 범위를 설정하고
	// 만들 때 위치도 설정한다음 
	// Attack Object에 Tag를 입력하면 됩니다
	// Attacker 도 Attack을 소환하는 오브젝트를 넣어주세요~
	class Attack : public fq::game_module::Component
	{
	public:
		Attack();
		~Attack();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		float GetAttackPower() const { return mAttackPower; }
		fq::game_module::GameObject* GetAttacker() const { return mAttacker; }
		DirectX::SimpleMath::Vector3 GetAttackDirection() const { return mAttackDirection; }
		DirectX::SimpleMath::Vector3 GetAttackPosition() const { return mAttackPosition; }

		/// <summary>
		/// 공격의 밀어내는 힘을 반환합니다
		/// </summary>
		float GetKnockBackPower() const { return mKnockBackPower; }

		/// <summary>
		/// 공격에 넉백이 있는지지 반환합니다 
		/// </summary>
		bool HasKnockBack()const;

		EKnockBackType GetKnockBackType()const { return mKnockBackType; }

		/// <summary>
		/// 공격 정보를 설정합니다 
		/// </summary>
		void Set(const AttackInfo& info);

		/// <summary>
		/// 공격카운트를 계산하고 공격이 활성화되어있는지 확인합니다
		/// </summary>
		bool ProcessAttack();

		/// <summary>
		/// 피격 사운드를 재생합니다 
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
		// 공격력
		float mAttackPower;
		float mDestroyTime;
		float mDestroyElapsedTime;

		// 공격횟수가 무한한지 
		bool mbIsInfinite;
		unsigned int mRemainingAttackCount;

		EKnockBackType mKnockBackType;
		DirectX::SimpleMath::Vector3 mAttackDirection; // 공격 방향
		DirectX::SimpleMath::Vector3 mAttackPosition;
		// 공격방향으로 밀어내는 힘
		float mKnockBackPower;

		// 공격을 하는 객체 
		fq::game_module::GameObject* mAttacker;
		std::string mHitSound;
		std::function<void()> mHitCallback;

		float mTargetPosRatio; // TargetPositionAndDirectionByAngle용 변수
		float mDirectionRatio; // TargetPositionAndDirectionByAngle용 변수

		std::string mAttackEffectEvent;

		friend void RegisterMetaData();
	};
}

