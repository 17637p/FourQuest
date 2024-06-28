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
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void Attack();
		void SummonSoul();

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

		float mMaxHp;
		float mHp;
		float mAttackPower;
		float mSoulStack;

		float mDashElapsedTime;
		float mInvincibleElapsedTime;

		float mInvincibleTime; // �����ð� 
		float mDashCoolTime; // �뽬��Ÿ��

		game_module::PrefabResource mAttackPrafab;
		game_module::PrefabResource mSoulPrefab;

		DirectX::SimpleMath::Vector3 mAttackRange;
		float mAttackPositionOffset;

		friend void RegisterMetaData();
	};


}