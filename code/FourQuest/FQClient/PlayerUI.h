#pragma once

#include "../FQGameModule/Component.h"
#include "PlayerDefine.h"

namespace fq::game_module
{
	class ImageUI;
	class ScreenManager;
}

namespace fq::client
{
	class Player;

	class PlayerUI : public fq::game_module::Component
	{
	public:
		PlayerUI();
		~PlayerUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		int GetPlayerID() const { return mPlayerID; }
		void SetPlayerID(int val) { mPlayerID = val; }

		void SetPlayer();
		void SetSoulGauge(float ratio);
		void SetHPBar(float ratio);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		// 무기랑 스킬아이콘 어떤 것을 렌더하거나 안할지 
		void setWeaponAndSkillIcons(int index, bool isRender);
		void setSkillCoolTime();

		void SetPlayerStateUpdate();

	private:
		int mPlayerID;
		fq::client::Player* mPlayer;

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
		game_module::ImageUI* mPlayerState;				 // 플레이어 상태

		game_module::ScreenManager* mScreenManager;

		friend void RegisterMetaData();
	};
}

// 검방, 완드, 활, 해머 순서로 