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

		// ����� ��ų������ � ���� �����ϰų� ������ 
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
		game_module::ImageUI* mHPBarGauge; // HP ���� ���� 

		// CoolTime ����
		float mCoolTimeHeight;
		game_module::ImageUI* mXCoolTimeImage;
		game_module::ImageUI* mACoolTimeImage;
		game_module::ImageUI* mRCoolTimeImage;

		int mSoulGaugeDegree;
		game_module::ImageUI* mSoulGauge; // Soul Gauge ���� 25 - 255
		game_module::ImageUI* mSoulIcon; // Soul Gauge ���� �ֱ�

		std::vector<game_module::ImageUI*> mWeaponIcons; // ���� ������

		std::vector<game_module::ImageUI*> mSkillIconXs; // ��ų ������
		std::vector<game_module::ImageUI*> mSkillIconAs; // ��ų ������ 
		std::vector<game_module::ImageUI*> mSkillIconRs; // ��ų ������ 

		std::vector<game_module::ImageUI*> mSoulSkillIcons; // ��ų ������ 

		std::vector<game_module::SpriteAnimationUI*> mSoulSprites;
		game_module::ImageUI* mPlayerState;				 // �÷��̾� ����
		game_module::ImageUI* mRetireMask;				 // ��Ÿ�̾� �̹���

		game_module::ScreenManager* mScreenManager;

		// ü�� ���� ����
		float mDecreaseOffset;
		float mDeceraseTime;
		float mDecreaseSpeed;
		float mDecreaseRatio;

		game_module::EventHandler mDecreaseHPRatioHandler;

		friend void RegisterMetaData();
	};
}

// �˹�, �ϵ�, Ȱ, �ظ� ������ 