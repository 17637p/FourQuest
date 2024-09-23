#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ImageUI;
	class ScreenManager;
}

namespace fq::client
{
	class BossMonster;

	class BossHP : public fq::game_module::Component
	{
	public:
		BossHP();
		~BossHP();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		void SetBoss(BossMonster* boss) { mBoss = boss; };

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		float mHPWidth;
		game_module::ImageUI* mHPBarGauge; // HP 비율 조정 

		float mShieldWidth;
		game_module::ImageUI* mShieldGauge; // HP 비율 조정 

		BossMonster* mBoss;

		game_module::ScreenManager* mScreenManager;
	};

}
