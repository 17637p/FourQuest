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
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 데드 아머를 소환합니다.
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
		/// 갑옷타입을 반환합니다
		/// </summary>
		fq::client::EArmourType GetArmourType() const { return mArmourType; }

		/// <summary>
		/// 스태프 영혼 공격을 방출합니다
		/// </summary>
		void EmitStaffSoulAttack();

		/// <summary>
		/// 검 영혼 공격을 방출합니다
		/// </summary>
		void EmitSwordSoulAttack();

		/// <summary>
		/// 활 영혼 공격을 방출합니다
		/// </summary>
		void EmitBowSoulAttack();

		/// <summary>
		/// 도끼 영혼 공격을 방출합니다
		/// </summary>
		void EmitAxeSoulAttack();

		/// <summary>
		/// 소울타입에 해당하는 무기를 장착합니다
		/// </summary>
		void EquipSoulWeapone();

		/// <summary>
		/// 갑옷 타입에 해당하는 무기를 장착합니다
		/// </summary>
		void EquipArmourWeapone();

		/// <summary>
		/// 소울 스택을 더합니다 
		/// </summary>
		void AddSoulGauge(float soul);

		/// <summary>
		/// Hp를 설정합니다 
		/// </summary>
		void SetHp(float hp);

		/// <summary>
		/// Hp를 읽어옵니다. 
		/// </summary>
		float GetHp() const;

		/// <summary>
		/// MaxHp를 설정합니다 
		/// </summary>
		void SetMaxHp(float maxHp);

		/// <summary>
		/// MaxHp를 읽어옵니다
		/// </summary>
		float GetMaxHp() const;

		/// <summary>
		/// Hp와 MaxHp를 한 번에 설정합니다.
		/// Editor에서 Set 처리를 위한 함수입니다.
		/// </summary>
		void SetEditorHp(float hp);

		/// <summary>
		/// 플레이어 Hp를 감소시킵니다
		/// </summary>
		/// <param name="hp">hp 감소량</param>
		/// <param name="bUseMinHp">Hp최소량 설정 사용여부</param>
		/// <param name="bIgnoreInvisible">무적시간에 무시하는지</param>
		void DecreaseHp(float hp, bool bUseMinHp = false, bool bIgnoreInvincible = false);

		/// <summary>
		/// 소울 공격이 가능한 상태인지 확인합니다.
		/// </summary>
		bool CanUseSoulAttack()const;

		/// <summary>
		/// 현재 피버타임인지 반환합니다
		/// </summary>
		bool IsFeverTime()const;

		/// <summary>
		/// 하체 애니메이션을 상태를 현재 입력 방향에 따라서 설정합니다. 
		/// </summary>
		void SetLowerBodyAnimation();

		/// <summary>
		/// A 스킬 쿨타임 비율을 반환합니다
		/// 스킬을 사용가능한 상태가 0.f
		/// </summary>
		float GetASkillCoolTimeRatio()const;
		void SetASkillCoolTimeRatio(float ratio);

		/// <summary>
		/// X 스킬 쿨타임 비율을 반환합니다 
		/// 스킬을 사용가능한 상태가 0.f
		/// </summary>
		float GetXSkillCoolTimeRatio()const;
		void SetXSkillCoolTimeRatio(float ratio);

		/// <summary>
		/// R 스킬 쿨타임 비율을 반환합니다 
		/// 스킬을 사용가능한 상태가 0.f
		/// </summary>
		float GetRSkillCoolTimeRatio()const;
		void SetRSkillCoolTimeRatio(float ratio);

		/// <summary>
		/// 초록색 림라이트 활성화
		/// </summary>
		void SetPoisonRimLight(float duration);

		/// <summary>
		/// Goddess Buff 
		/// </summary>
		/// <returns></returns>
		bool GetGBDecreaseDurability() const { return mGBDecreaseDurability; } // 내구도 감소  
		void SetGBDecreaseDurability(bool val) { mGBDecreaseDurability = val; }

		float GetGBIncreaseAttackPower() const { return mGBIncreaseAttackPower; } // 공격력 강화
		void SetGBIncreaseAttackPower(float val) { mGBIncreaseAttackPower = val; } // 버프 없을 때 0

		float GetGBDecreaseCooltime() const { return mGBDecreaseCooltime; } // 쿨타임 감소
		void SetGBDecreaseCooltime(float val) { mGBDecreaseCooltime = val; } // 버프 없을 때 1

		float GetGBIncreaseSpeed() const { return mGBIncreaseSpeed; } // 이동속도 증가 
		void SetGBIncreaseSpeed(float val) { mGBIncreaseSpeed = val; } // 버프 없을 때 0

		/// <summary>
		/// 플레이어 위치에 더미 오브젝트를 생성합니다.
		/// 소울 상태로 변환된 경우에만 더미 생성이 가능하고 그 외에는 nullptr을 반환합니다.
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
		void checkPoisonDuration(float dt); // 독 림라이트 지속시간 체크해서 off
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

		ESoulType mSoulType; // 영혼 타입
		ESoulType mEquipWeapone;
		EArmourType mArmourType;

		float mMaxHp; // 최대 체력
		float mHp; // 현재 체력
		float mMaxSoulGauge; // 최대 소울 게이지
		float mSoulGauge; // 소울 게이지
		float mAttackPower; // 공격력

		float mBaseAcceleration;	// 기본 가속력
		float mBaseSpeed;	// 기본 스피드
		float mBaseAttackPower; // 기본 공격력
		int mSoulBuffNumber; // 영혼 강화 갯수

		float mInvincibleElapsedTime; // 무적 경과 시간
		float mInvincibleTime; // 무적시간 
		float mFeverTime; // 갑옷 버프 시간
		float mFeverElapsedTime; // 피버타임 경과 시간
		bool mbIsFeverTime; // 현재 피버 타임인지 

		float mRSkillCoolTimeRatio; // R 버튼 쿨타임  
		float mASkillCoolTimeRatio; // A 버튼 쿨타임
		float mXSkillCoolTimeRatio; // X 버튼 쿨타임

		float mPreRSkillCoolTimeRatio; // R 버튼 쿨타임  
		float mPreASkillCoolTimeRatio; // A 버튼 쿨타임
		float mPreXSkillCoolTimeRatio; // X 버튼 쿨타임

		float mUnequipArmourDurationTime;	// 갑옷 벗고 있는 시간
		bool mbIsUnequipArmourButton;			// 갑옷 벗는 버튼 클릭

		game_module::PrefabResource mSoulPrefab; // 영혼화 프리팹 
		game_module::PrefabResource mSwordSoulAttack;
		game_module::PrefabResource mAxeSoulAttack; // 도끼 영혼 공격 
		game_module::PrefabResource mBowSoulAttack; // 활 영혼 공격 	
		game_module::PrefabResource mStaffSoulAttack; // 마법 영혼 공격
		game_module::PrefabResource mDeadKnightArmour;	// 기사 갑옷
		game_module::PrefabResource mDeadMagicArmour;	// 마법사 갑옷
		game_module::PrefabResource mDeadArcherArmour;	// 궁수 갑옷
		game_module::PrefabResource mDeadWarriorArmour; // 광전사 갑옷

		// 영혼 머리 
		game_module::PrefabResource mSwordHaed;
		game_module::PrefabResource mBowHaed;
		game_module::PrefabResource mAxeHaed;
		game_module::PrefabResource mStaffHaed;

		bool mbOnShieldBlock;
		bool mbIsActiveOnHit; // 워리어 갑옷 차징과 돌진 시 OnHit로 전이가 불가능하여 추가

		// 독 림라이트
		float mDuration;
		float mCurTime;

		// Goddess Buff
		bool mGBDecreaseDurability; // 갑옷 내구도 감소 여부
		float mGBIncreaseAttackPower; // 공격력 증가
		float mGBDecreaseCooltime; // 쿨타임 감소
		float mGBIncreaseSpeed; // 이동속도 증가 

		game_module::SkinnedMeshRenderer* mSkinnedMesh;

		std::array<game_module::StaticMeshRenderer*, static_cast<int>(EWeaponeMesh::End)> mWeaponeMeshes;
		friend void RegisterMetaData();

		// 이펙트 방출 여부
		bool mbIsEmitEnhanceEffect;

		// 더미 프리팹
		bool mbCanCreateDummy;
		game_module::PrefabResource mDummyPrefab;

		float mKnockBackTime;
		float mKnockBackPower;
		DirectX::SimpleMath::Vector3 mKnockBackDir;
	};
}