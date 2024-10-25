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

		void SetBoss(BossMonster* boss);
		
		/// <summary>
		/// Hp감소연출 사용여부를 설정합니다.
		/// </summary>
		void SetUseDecreaseEffet(bool useDecreaseEffect);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		bool mUseDecreaseEffect; // Hp 감소 효과 사용 여부 

		float mHPWidth;
		float mShieldWidth;
		float mDecreaseHpRatio;
		float mPrevHpRatio;

		BossMonster* mBoss;
		game_module::ImageUI* mHPBarGauge; // HP 비율 조정 
		game_module::ImageUI* mDecreaseGauge; // HP 감소 효과 
		game_module::ImageUI* mShieldGauge; // 그로기 비율 조정 
		game_module::ScreenManager* mScreenManager;
	};

}
