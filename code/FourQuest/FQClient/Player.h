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
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void Attack();
		
		float GetHp() const { return mHp; }
		void SetHp(float hp) { mHp = hp; }
		fq::game_module::PrefabResource GetAttackPrefab() const { return mAttack; }
		void SetAttackPrefab(fq::game_module::PrefabResource val) { mAttack = val; }

		void OnTriggerEnter(const game_module::Collision& collision) override;

	private:
		void processDash();

		void OnStart() override;
		void OnDestroy() override;
		
		void OnUpdate(float dt) override;

		entt::meta_handle GetHandle() override { return *this; }
	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;

		float mHp;
		float mAttackPower;

		game_module::PrefabResource mAttack;
	};


}