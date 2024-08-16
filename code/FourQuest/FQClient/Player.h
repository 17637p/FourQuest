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
		/// 복사본을 반환합니다 
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

		ESoulType mSoulType; // 영혼 타입
		ESoulType mEquipWeapone;
		EArmourType mArmourType;

		float mMaxHp; // 최대 체력
		float mHp; // 현재 체력
		float mMaxSoulStack; // 최대 소울 게이지
		float mSoulStack; // 소울 게이지
		float mAttackPower; // 공격력
		float mAttackSpeed; // 공격속도

		float mInvincibleElapsedTime; // 무적 경과 시간
		float mInvincibleTime; // 무적시간 
		float mFeverTime; // 갑옷 버프 시간

		game_module::PrefabResource mSoulPrefab; // 영혼화 프리팹 
		game_module::PrefabResource mSwordSoulAttack;
		game_module::PrefabResource mAxeSoulAttack; // 도끼 영혼 공격 
		game_module::PrefabResource mBowSoulAttack; // 활 영혼 공격 	
		game_module::PrefabResource mStaffSoulAttack; // 마법 영혼 공격

		// 도끼 영혼 공격 관련
		float mAxeAttackTick;

		bool mbOnShieldBlock;

		std::array<game_module::StaticMeshRenderer*, static_cast<int>(EWeaponeMesh::End)> mWeaponeMeshes;
		friend void RegisterMetaData();
	};
}