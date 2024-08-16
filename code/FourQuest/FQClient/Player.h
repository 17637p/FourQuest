#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "PlayerDefine.h"

namespace fq::game_module
{
	class Animator;
	class CharacterController;
	class Transform;
	class StaticMeshRenderer;
}

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
		float GetHPRatio() const { return mHp / mMaxHp; }
		float GetSoultStackRatio() const { return mSoulStack / mMaxSoulStack; }
		float GetPlayerID() const;

		void SetOnShieldBlock(bool val) { mbOnShieldBlock = val; }

		/// <summary>
		/// ����Ÿ���� ��ȯ�մϴ�
		/// </summary>
		fq::client::EArmourType GetArmourType() const { return mArmourType; }

		/// <summary>
		/// ������ ��ȥ ������ �����մϴ�
		/// </summary>
		void EmitStaffSoulAttack();

		/// <summary>
		/// �� ��ȥ ������ �����մϴ�
		/// </summary>
		void EmitSwordSoulAttack();

		/// <summary>
		/// Ȱ ��ȥ ������ �����մϴ�
		/// </summary>
		void EmitBowSoulAttack();

		/// <summary>
		/// ���� ��ȥ ������ �����մϴ�
		/// </summary>
		void EmitAxeSoulAttack();

		/// <summary>
		/// �ҿ�Ÿ�Կ� �ش��ϴ� ���⸦ �����մϴ�
		/// </summary>
		void EquipSoulWeapone();

		/// <summary>
		/// ���� Ÿ�Կ� �ش��ϴ� ���⸦ �����մϴ�
		/// </summary>
		void EquipArmourWeapone();

	private:
		void processInput();
		void processCoolTime(float dt);
		void processFeverTime(float dt);
		void equipWeapone(ESoulType equipType, bool isEquip);
		void linkWeaponeMeshes();

		void OnStart() override;
		void OnDestroy() override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;

		entt::meta_handle GetHandle() override { return *this; }

	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;
		game_module::Transform* mTransform;

		ESoulType mSoulType; // ��ȥ Ÿ��
		ESoulType mEquipWeapone;
		EArmourType mArmourType;

		float mMaxHp; // �ִ� ü��
		float mHp; // ���� ü��
		float mMaxSoulStack; // �ִ� �ҿ� ������
		float mSoulStack; // �ҿ� ������
		float mAttackPower; // ���ݷ�
		float mAttackSpeed; // ���ݼӵ�

		float mInvincibleElapsedTime; // ���� ��� �ð�
		float mInvincibleTime; // �����ð� 
		float mFeverTime; // ���� ���� �ð�

		game_module::PrefabResource mSoulPrefab; // ��ȥȭ ������ 
		game_module::PrefabResource mSwordSoulAttack;
		game_module::PrefabResource mAxeSoulAttack; // ���� ��ȥ ���� 
		game_module::PrefabResource mBowSoulAttack; // Ȱ ��ȥ ���� 	
		game_module::PrefabResource mStaffSoulAttack; // ���� ��ȥ ����

		// ���� ��ȥ ���� ����
		float mAxeAttackTick;

		bool mbOnShieldBlock;

		std::array<game_module::StaticMeshRenderer*, static_cast<int>(EWeaponeMesh::End)> mWeaponeMeshes;
		friend void RegisterMetaData();
	};
}