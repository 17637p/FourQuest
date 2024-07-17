#pragma once

#include "../FQGameModule/GameModule.h"
#include "PlayerDefine.h"

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

		void SummonSoul();

		fq::client::ESoulType GetSoulType() const { return mSoulType; }
		void SetSoulType(fq::client::ESoulType val) { mSoulType = val; }
	
		float GetAttackPower() const { return mAttackPower; }
	private:
		void processInput();
		void processCoolTime(float dt);
		void processFeverTime(float dt);

		void OnStart() override;
		void OnDestroy() override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;

		entt::meta_handle GetHandle() override { return *this; }

	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;

		ESoulType mSoulType; // ��ȥ Ÿ��

		float mMaxHp; // �ִ� ü��
		float mHp; // ���� ü��
		float mSoulStack; // �ҿ� ������
		float mAttackPower; // ���ݷ�
		
		float mInvincibleElapsedTime; // ���� ��� �ð�

		float mInvincibleTime; // �����ð� 

		float mFeverTime; // ���� ���� �ð�

		game_module::PrefabResource mAttackPrafab; 
		game_module::PrefabResource mSoulPrefab; // ��ȥȭ ������ 

		float mAttackPositionOffset; 

		friend void RegisterMetaData();
	};
}