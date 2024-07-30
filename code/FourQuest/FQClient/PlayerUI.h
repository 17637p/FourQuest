#pragma once

#include "../FQGameModule/Component.h"
#include "PlayerDefine.h"

namespace fq::game_module
{
	class ImageUI;
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

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		// ����� ��ų������ � ���� �����ϰų� ������ 
		void SetWeaponAndSkillIcons(int index, bool isRender);

	private:
		int mPlayerID;
		fq::client::Player* mPlayer;

		game_module::ImageUI* mHPBarGauge; // HP ���� ���� 
		float mHPWidth;

		std::vector<game_module::ImageUI*> mWeaponIcons; // ���� ������

		std::vector<game_module::ImageUI*> mSkillIconXs; // ��ų ������
		std::vector<game_module::ImageUI*> mSkillIconAs; // ��ų ������ 
		std::vector<game_module::ImageUI*> mSkillIconRs; // ��ų ������ 

		game_module::ScreenManager* mScreenManager;

		friend void RegisterMetaData();
	};
}

// �˹�, �ϵ�, Ȱ, �ظ� ������ 