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

		// 무기랑 스킬아이콘 어떤 것을 렌더하거나 안할지 
		void SetWeaponAndSkillIcons(int index, bool isRender);

	private:
		int mPlayerID;
		fq::client::Player* mPlayer;

		game_module::ImageUI* mHPBarGauge; // HP 비율 조정 
		float mHPWidth;

		std::vector<game_module::ImageUI*> mWeaponIcons; // 무기 아이콘

		std::vector<game_module::ImageUI*> mSkillIconXs; // 스킬 아이콘
		std::vector<game_module::ImageUI*> mSkillIconAs; // 스킬 아이콘 
		std::vector<game_module::ImageUI*> mSkillIconRs; // 스킬 아이콘 

		game_module::ScreenManager* mScreenManager;

		friend void RegisterMetaData();
	};
}

// 검방, 완드, 활, 해머 순서로 