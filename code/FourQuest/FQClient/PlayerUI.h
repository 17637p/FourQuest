#pragma once

#include "../FQGameModule/GameModule.h"

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

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		int mPlayerID;
		fq::client::Player* mPlayer;

		game_module::ImageUI* mHPBarGauge; // HP ���� ���� 
		float mHPWidth;

		game_module::ImageUI* mWeaponIcon; // ���� ������
		std::vector<std::string> mWeaponIcons;

		game_module::ImageUI* mSkillIconX; // ��ų ������
		std::vector<std::string> mSkillIconsXs;
		game_module::ImageUI* mSkillIconA; // ��ų ������ 
		std::vector<std::string> mSkillIconsAs;
		game_module::ImageUI* mSkillIconR; // ��ų ������ 
		std::vector<std::string> mSkillIconsRs;

		friend void RegisterMetaData();
	};
}

// �˹�, �ϵ�, Ȱ, �ظ� ������ 