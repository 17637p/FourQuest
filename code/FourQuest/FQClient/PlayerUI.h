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

		game_module::ImageUI* mHPBarGauge; // HP 비율 조정 
		float mHPWidth;

		game_module::ImageUI* mWeaponIcon; // 무기 아이콘
		std::vector<std::string> mWeaponIcons;

		game_module::ImageUI* mSkillIconX; // 스킬 아이콘
		std::vector<std::string> mSkillIconsXs;
		game_module::ImageUI* mSkillIconA; // 스킬 아이콘 
		std::vector<std::string> mSkillIconsAs;
		game_module::ImageUI* mSkillIconR; // 스킬 아이콘 
		std::vector<std::string> mSkillIconsRs;

		friend void RegisterMetaData();
	};
}

// 검방, 완드, 활, 해머 순서로 