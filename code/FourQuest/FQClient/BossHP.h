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
		/// Hp���ҿ��� ��뿩�θ� �����մϴ�.
		/// </summary>
		void SetUseDecreaseEffet(bool useDecreaseEffect);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		bool mUseDecreaseEffect; // Hp ���� ȿ�� ��� ���� 

		float mHPWidth;
		float mShieldWidth;
		float mDecreaseHpRatio;
		float mPrevHpRatio;

		BossMonster* mBoss;
		game_module::ImageUI* mHPBarGauge; // HP ���� ���� 
		game_module::ImageUI* mDecreaseGauge; // HP ���� ȿ�� 
		game_module::ImageUI* mShieldGauge; // �׷α� ���� ���� 
		game_module::ScreenManager* mScreenManager;
	};

}
