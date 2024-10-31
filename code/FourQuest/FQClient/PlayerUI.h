#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/EventHandler.h"
#include "PlayerDefine.h"

namespace fq::game_module
{
	class ImageUI;
	class ScreenManager;
	class SpriteAnimationUI;
}

namespace fq::client
{
	class Player;
	class Soul;
	class GameManager;

	class PlayerUI : public fq::game_module::Component
	{
	public:
		PlayerUI();
		~PlayerUI();

		PlayerUI(const PlayerUI& other);
		PlayerUI& operator=(const PlayerUI& other);

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnDestroy() override;

		int GetPlayerID() const { return mPlayerID; }
		void SetPlayerID(int val) { mPlayerID = val; }

		void SetPlayer(fq::client::GameManager* gameMgr);
		void SetSoulGauge(float ratio);
		void SetHPBar(float ratio);
		void SetSoulSprite(int index, bool isOn);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		// 무기랑 스킬아이콘 어떤 것을 렌더하거나 안할지 
		void setWeaponAndSkillIcons(int index, bool isRender);
		void setSoulSkillIcon();
		void setSkillCoolTime();
		void resetSkillCoolTime();
		void setPlayerStateUpdate();

		void eventProcessDecreaseHPRatio();

	private:
		int mPlayerID;
		fq::client::Player* mPlayer;
		fq::client::Soul* mSoul;
		 
		float mHPWidth;
		game_module::ImageUI* mHPBarGauge; // HP 비율 조정 

		// CoolTime 적용
		float mCoolTimeHeight;
		game_module::ImageUI* mXCoolTimeImage;
		game_module::ImageUI* mACoolTimeImage;
		game_module::ImageUI* mRCoolTimeImage;

		int mSoulGaugeDegree;
		game_module::ImageUI* mSoulGauge; // Soul Gauge 조정 25 - 255
		game_module::ImageUI* mSoulIcon; // Soul Gauge 끝에 넣기

		std::vector<game_module::ImageUI*> mWeaponIcons; // 무기 아이콘

		std::vector<game_module::ImageUI*> mSkillIconXs; // 스킬 아이콘
		std::vector<game_module::ImageUI*> mSkillIconAs; // 스킬 아이콘 
		std::vector<game_module::ImageUI*> mSkillIconRs; // 스킬 아이콘 

		std::vector<game_module::ImageUI*> mSoulSkillIcons; // 스킬 아이콘 

		std::vector<game_module::SpriteAnimationUI*> mSoulSprites;
		game_module::ImageUI* mPlayerState;				 // 플레이어 상태
		game_module::ImageUI* mRetireMask;				 // 리타이어 이미지

		game_module::ScreenManager* mScreenManager;

		// 체력 감소 연출
		float mDecreaseOffset;
		float mDeceraseTime;
		float mDecreaseSpeed;
		float mDecreaseRatio;

		game_module::EventHandler mDecreaseHPRatioHandler;

		friend void RegisterMetaData();
	};
}

// 검방, 완드, 활, 해머 순서로 