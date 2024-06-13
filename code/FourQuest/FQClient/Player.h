#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class Player
		: public fq::game_module::Component
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

		fq::game_module::PrefabResource GetAttackPrefab() const { return mAttack; }
		void SetAttackPrefab(fq::game_module::PrefabResource val) { mAttack = val; }
		fq::game_module::PrefabResource GetSoulPrefab() const { return mSoul; }
		void SetSoulPrefab(fq::game_module::PrefabResource val) { mSoul = val; }
	private:
		void processInput();

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

		game_module::PrefabResource mAttack;
		game_module::PrefabResource mSoul;
	};


}