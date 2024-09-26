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

		// ����� ��ų������ � ���� �����ϰų� ������ 
		void setWeaponAndSkillIcons(int index, bool isRender);
		void setSkillCoolTime();

		void SetPlayerStateUpdate();

	private:
		int mPlayerID;
		fq::client::Player* mPlayer;

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
		game_module::ImageUI* mPlayerState;				 // �÷��̾� ����

		game_module::ScreenManager* mScreenManager;

		friend void RegisterMetaData();
	};
}

// �˹�, �ϵ�, Ȱ, �ظ� ������ 