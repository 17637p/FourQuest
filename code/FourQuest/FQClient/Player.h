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
	class SkinnedMeshRenderer;
}

namespace fq::client
{
	class PlayerHPBar;
	class BGaugeUI;

	class Player : public fq::game_module::Component
	{
	public:
		Player();
		~Player();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// ���� �ƸӸ� ��ȯ�մϴ�.
		/// </summary>
		void RemainDeadArmour();

		void SummonSoul(bool isDestroyArmour);

		fq::client::ESoulType GetSoulType() const { return mSoulType; }
		void SetSoulType(fq::client::ESoulType val) { mSoulType = val; }

		float GetAttackPower() const { return mAttackPower; }
		float GetHPRatio() const { return mHp / mMaxHp; }
		float GetSoultGaugeRatio() const { return mSoulGauge / mMaxSoulGauge; }
		int GetPlayerID() const;

		void SetOnShieldBlock(bool val) { mbOnShieldBlock = val; }
		void SetIsActiveOnHit(bool bIsActiveOnHit) { mbIsActiveOnHit = bIsActiveOnHit; }
		int GetSoulBuffNumber() { return mSoulBuffNumber; }
		void SetSoulBuffNumber(int buffNumber) { mSoulBuffNumber = buffNumber; }

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

		/// <summary>
		/// �ҿ� ������ ���մϴ� 
		/// </summary>
		void AddSoulGauge(float soul);

		/// <summary>
		/// Hp�� �����մϴ� 
		/// </summary>
		void SetHp(float hp);

		/// <summary>
		/// Hp�� �о�ɴϴ�. 
		/// </summary>
		float GetHp() const;

		/// <summary>
		/// MaxHp�� �����մϴ� 
		/// </summary>
		void SetMaxHp(float maxHp);

		/// <summary>
		/// MaxHp�� �о�ɴϴ�
		/// </summary>
		float GetMaxHp() const;

		/// <summary>
		/// Hp�� MaxHp�� �� ���� �����մϴ�.
		/// Editor���� Set ó���� ���� �Լ��Դϴ�.
		/// </summary>
		void SetEditorHp(float hp);

		/// <summary>
		/// �÷��̾� Hp�� ���ҽ�ŵ�ϴ�
		/// </summary>
		/// <param name="hp">hp ���ҷ�</param>
		/// <param name="bUseMinHp">Hp�ּҷ� ���� ��뿩��</param>
		/// <param name="bIgnoreInvisible">�����ð��� �����ϴ���</param>
		void DecreaseHp(float hp, bool bUseMinHp = false, bool bIgnoreInvincible = false);

		/// <summary>
		/// �ҿ� ������ ������ �������� Ȯ���մϴ�.
		/// </summary>
		bool CanUseSoulAttack()const;

		/// <summary>
		/// ���� �ǹ�Ÿ������ ��ȯ�մϴ�
		/// </summary>
		bool IsFeverTime()const;

		/// <summary>
		/// ��ü �ִϸ��̼��� ���¸� ���� �Է� ���⿡ ���� �����մϴ�. 
		/// </summary>
		void SetLowerBodyAnimation();

		/// <summary>
		/// A ��ų ��Ÿ�� ������ ��ȯ�մϴ�
		/// ��ų�� ��밡���� ���°� 0.f
		/// </summary>
		float GetASkillCoolTimeRatio()const;
		void SetASkillCoolTimeRatio(float ratio);

		/// <summary>
		/// X ��ų ��Ÿ�� ������ ��ȯ�մϴ� 
		/// ��ų�� ��밡���� ���°� 0.f
		/// </summary>
		float GetXSkillCoolTimeRatio()const;
		void SetXSkillCoolTimeRatio(float ratio);

		/// <summary>
		/// R ��ų ��Ÿ�� ������ ��ȯ�մϴ� 
		/// ��ų�� ��밡���� ���°� 0.f
		/// </summary>
		float GetRSkillCoolTimeRatio()const;
		void SetRSkillCoolTimeRatio(float ratio);

		/// <summary>
		/// �ʷϻ� ������Ʈ Ȱ��ȭ
		/// </summary>
		void SetPoisonRimLight(float duration);

		/// <summary>
		/// Goddess Buff 
		/// </summary>
		/// <returns></returns>
		bool GetGBDecreaseDurability() const { return mGBDecreaseDurability; } // ������ ����  
		void SetGBDecreaseDurability(bool val) { mGBDecreaseDurability = val; }

		float GetGBIncreaseAttackPower() const { return mGBIncreaseAttackPower; } // ���ݷ� ��ȭ
		void SetGBIncreaseAttackPower(float val) { mGBIncreaseAttackPower = val; } // ���� ���� �� 0

		float GetGBDecreaseCooltime() const { return mGBDecreaseCooltime; } // ��Ÿ�� ����
		void SetGBDecreaseCooltime(float val) { mGBDecreaseCooltime = val; } // ���� ���� �� 1

		float GetGBIncreaseSpeed() const { return mGBIncreaseSpeed; } // �̵��ӵ� ���� 
		void SetGBIncreaseSpeed(float val) { mGBIncreaseSpeed = val; } // ���� ���� �� 0

		/// <summary>
		/// �÷��̾� ��ġ�� ���� ������Ʈ�� �����մϴ�.
		/// �ҿ� ���·� ��ȯ�� ��쿡�� ���� ������ �����ϰ� �� �ܿ��� nullptr�� ��ȯ�մϴ�.
		/// </summary>
		fq::game_module::GameObject* CreateDummyOrNull();

		void SetSoulGauge(float soulGauge) { mSoulGauge = soulGauge; }

	private:
		void processKnockBack(float dt);
		void processInput(float dt);
		void processCoolTime(float dt);
		void processFeverTime(float dt);
		void processDebuff(float dt);
		void processBuff();
		void equipWeapone(ESoulType equipType, bool isEquip);
		void linkWeaponeMeshes();
		void setFeverBuff(bool isFever);
		void setDecalColor();
		void linkSoulTypeHead();
		void checkPoisonDuration(float dt); // �� ������Ʈ ���ӽð� üũ�ؼ� off
		void checkCoolTime();

		void OnStart() override;
		void OnDestroy() override;
		void OnUpdate(float dt) override;
		void OnLateUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerStay(const game_module::Collision& collision) override;

		void playBowSoulSound();
		void handleEmitEnhanceEffect();

		entt::meta_handle GetHandle() override { return *this; }

	private:
		game_module::Animator* mAnimator;
		game_module::CharacterController* mController;
		game_module::Transform* mTransform;
		BGaugeUI* mBGaugeUI;
		PlayerHPBar* mPlayerHpBar;

		ESoulType mSoulType; // ��ȥ Ÿ��
		ESoulType mEquipWeapone;
		EArmourType mArmourType;

		float mMaxHp; // �ִ� ü��
		float mHp; // ���� ü��
		float mMaxSoulGauge; // �ִ� �ҿ� ������
		float mSoulGauge; // �ҿ� ������
		float mAttackPower; // ���ݷ�

		float mBaseAcceleration;	// �⺻ ���ӷ�
		float mBaseSpeed;	// �⺻ ���ǵ�
		float mBaseAttackPower; // �⺻ ���ݷ�
		int mSoulBuffNumber; // ��ȥ ��ȭ ����

		float mInvincibleElapsedTime; // ���� ��� �ð�
		float mInvincibleTime; // �����ð� 
		float mFeverTime; // ���� ���� �ð�
		float mFeverElapsedTime; // �ǹ�Ÿ�� ��� �ð�
		bool mbIsFeverTime; // ���� �ǹ� Ÿ������ 

		float mRSkillCoolTimeRatio; // R ��ư ��Ÿ��  
		float mASkillCoolTimeRatio; // A ��ư ��Ÿ��
		float mXSkillCoolTimeRatio; // X ��ư ��Ÿ��

		float mPreRSkillCoolTimeRatio; // R ��ư ��Ÿ��  
		float mPreASkillCoolTimeRatio; // A ��ư ��Ÿ��
		float mPreXSkillCoolTimeRatio; // X ��ư ��Ÿ��

		float mUnequipArmourDurationTime;	// ���� ���� �ִ� �ð�
		bool mbIsUnequipArmourButton;			// ���� ���� ��ư Ŭ��

		game_module::PrefabResource mSoulPrefab; // ��ȥȭ ������ 
		game_module::PrefabResource mSwordSoulAttack;
		game_module::PrefabResource mAxeSoulAttack; // ���� ��ȥ ���� 
		game_module::PrefabResource mBowSoulAttack; // Ȱ ��ȥ ���� 	
		game_module::PrefabResource mStaffSoulAttack; // ���� ��ȥ ����
		game_module::PrefabResource mDeadKnightArmour;	// ��� ����
		game_module::PrefabResource mDeadMagicArmour;	// ������ ����
		game_module::PrefabResource mDeadArcherArmour;	// �ü� ����
		game_module::PrefabResource mDeadWarriorArmour; // ������ ����

		// ��ȥ �Ӹ� 
		game_module::PrefabResource mSwordHaed;
		game_module::PrefabResource mBowHaed;
		game_module::PrefabResource mAxeHaed;
		game_module::PrefabResource mStaffHaed;

		bool mbOnShieldBlock;
		bool mbIsActiveOnHit; // ������ ���� ��¡�� ���� �� OnHit�� ���̰� �Ұ����Ͽ� �߰�

		// �� ������Ʈ
		float mDuration;
		float mCurTime;

		// Goddess Buff
		bool mGBDecreaseDurability; // ���� ������ ���� ����
		float mGBIncreaseAttackPower; // ���ݷ� ����
		float mGBDecreaseCooltime; // ��Ÿ�� ����
		float mGBIncreaseSpeed; // �̵��ӵ� ���� 

		game_module::SkinnedMeshRenderer* mSkinnedMesh;

		std::array<game_module::StaticMeshRenderer*, static_cast<int>(EWeaponeMesh::End)> mWeaponeMeshes;
		friend void RegisterMetaData();

		// ����Ʈ ���� ����
		bool mbIsEmitEnhanceEffect;

		// ���� ������
		bool mbCanCreateDummy;
		game_module::PrefabResource mDummyPrefab;

		float mKnockBackTime;
		float mKnockBackPower;
		DirectX::SimpleMath::Vector3 mKnockBackDir;
	};
}