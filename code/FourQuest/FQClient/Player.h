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
		/// 복사본을 반환합니다 
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

		ESoulType mSoulType; // 영혼 타입

		float mMaxHp; // 최대 체력
		float mHp; // 현재 체력
		float mSoulStack; // 소울 게이지
		float mAttackPower; // 공격력
		
		float mInvincibleElapsedTime; // 무적 경과 시간

		float mInvincibleTime; // 무적시간 

		float mFeverTime; // 갑옷 버프 시간

		game_module::PrefabResource mAttackPrafab; 
		game_module::PrefabResource mSoulPrefab; // 영혼화 프리팹 

		float mAttackPositionOffset; 

		friend void RegisterMetaData();
	};
}