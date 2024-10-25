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
		/// 화살 오브젝트가 상대 오브젝트에 박히는 함수
		/// </summary>
		virtual void OnTriggerEnter(const game_module::Collision& collision) override;

		/// <summary>
		/// 화살 공격 세팅
		/// </summary>
		void Set(const ArrowAttackInfo& info);

		/// <summary>
		/// 화살 피격 사운드 재생
		/// </summary>
		void PlayHitSound();

		bool GetIsStrongAttack() const { return mbIsStrongAttack; }
		float GetWeakAttackPower() const { return mWeakAttackPower; }
		float GetStrongAttackPower() const { return mStrongAttackPower; }
		fq::game_module::GameObject* GetAttacker() const { return mAttacker; }
		DirectX::SimpleMath::Vector3 GetAttackDirection() const { return mAttackDirection; }
		DirectX::SimpleMath::Matrix GetAttackPosition() const { return mAttackTransform; }

	private:
		/// <summary>
		/// 화살의 라이프 타임을 계산하고 시간이 지나면 화살 오브젝트 삭제
		/// </summary>
		void OnUpdate(float dt) override;
		void OnStart() override;

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		// 강공격, 약공격 공격력 및 최대 관통 갯수
		bool  mbIsStrongAttack;
		float mWeakAttackPower;
		float mStrongAttackPower;
		float mWeakProjectileVelocity;
		float mStrongProjectileVelocity;
		DirectX::SimpleMath::Vector3 mAttackDirection;
		DirectX::SimpleMath::Matrix  mAttackTransform;
		int mMaxBlockCount;

		// 생존 시간
		float mLifeElapsedTime;
		float mLifeTime;

		// 공격을 하는 객체 
		fq::game_module::GameObject* mAttacker;
		std::string mHitSound;

		friend void RegisterMetaData();
	};
}