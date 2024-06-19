#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class Player : public fq::game_module::Component
	{
	public:
		Player();
		~Player();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void Attack();
		void SummonSoul();

		float GetHp() const { return mHp; }
		void SetHp(float hp) { mHp = hp; }

	private:
		void processInput();
		void processCoolTime(float dt);
		
		void OnStart() override;
		void OnDestroy() override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;

		entt::meta_handle GetHandle() override { return *this; }
	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;

		float mHp;
		float mAttackPower;
		float mSoulStack;

		float mDashElapsedTime;
		float mInvincibleElapsedTime;

		float mInvincibleTime; // 무적시간 
		float mDashCoolTime; // 대쉬쿨타임
		game_module::PrefabResource mAttackPrafab;
		game_module::PrefabResource mSoulPrefab;

		friend void RegisterMetaData();
	};


}