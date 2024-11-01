#pragma once

#include "../FQGameModule/Component.h"
#include "PlayerDefine.h"
#include "BGaugeUI.h"

namespace fq::game_module
{
	class CharacterController;
	class GameObject;
}

namespace fq::client
{
	class DeadArmour;
	class GoddessStatue;
	class PlayerHPBar;

	class Soul :public game_module::Component
	{
	public:
		Soul();
		~Soul();

		void DestorySoul();
		fq::client::ESoulType GetSoulType() const { return mSoulType; }
		void SetSoulType(fq::client::ESoulType type);

		void SetSoulColor();

		void ReleaseGoddessStatue();

		float GetSoulHpRatio()const;
		float GetSoulGauge() const { return mSoulGauge; }
		void SetSoulGauge(float soulGauge) { mSoulGauge = soulGauge; }
		float GetMaxSoulGauge() const { return mMaxSoulGauge; }
		void SetMaxSoulGauge(float maxSoulGauge) { mMaxSoulGauge = maxSoulGauge; }
		float GetSoulGaugeRatio() const { if (mMaxSoulGauge == 0.f) { return 0; } return mSoulGauge / mMaxSoulGauge; }

		/// <summary>
		/// 소울을 무적상태로 설정합니다.
		/// </summary>
		void SetInvincible();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnTriggerEnter(const fq::game_module::Collision& collision) override;
		void OnTriggerExit(const fq::game_module::Collision& collision) override;
		void OnUpdate(float dt) override;
		void OnLateUpdate(float dt) override;

		// 영혼 주변 갑옷 플레이어 검색 및 사정거리 안에 있으면 버프
		void checkOtherPlayer();
		// 영혼 인근 갑옷 상호작용
		void selectArmour();
		// 영혼 인근 여신상 상호작용
		void selectGoddessStatue(float dt);
		void checkReleaseGoddessStatue();
		// 영혼 HP 업데이트
		void updateSoulHP(float dt);
		// 갑옷 장착 중일 경우를 체크함
		bool handleOnSummon();

		// 소울 시작시 hp 설정 
		void setSoulHP();
		void setName();

		void processInput(float dt);

	private:
		game_module::CharacterController* mController;
		std::vector<DeadArmour*> mSelectArmours;
		GoddessStatue* mSelectGoddessStatue;

		PlayerHPBar* mPlayerHpBar;
		BGaugeUI* mBGaugeUI;
		ESoulType mSoulType;

		/// 체력 관련
		// 근처에 플레이어가 있는지
		// 세이프 존에 있는지
		float mHP;
		bool mbIsDistanceInPlayer;
		bool mbIsInSafeZone;

		/// 여신상 관련
		// 영혼 여신상에 빙의 중인지
		float mNeedHoldB;
		float mCurHoldB;
		bool mIsOverlayGoddessStatue;

		bool mbIsOnSummon;
		DeadArmour* mSummonArmourOrNull;

		// 영혼 게이지
		float mSoulGauge;
		float mMaxSoulGauge;

		// BGaugeUI 여신상과 갑옷 근처일 때 활성화하는 로직 작성을 위해 별도로 변수 추가
		bool mbIsVisibleBGaugeUI;
		float mBGaugeFillAmount;

		// 소울 대시 관련
		float mDashSpeed;
		float mDashElapsed;
		float mDashCoolTime;

	private:
		friend void RegisterMetaData();
	};

}
