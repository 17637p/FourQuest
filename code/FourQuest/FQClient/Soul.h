#pragma once

#include "../FQGameModule/Component.h"
#include "PlayerDefine.h"
#include "BGaugeUI.h"

namespace fq::game_module
{
	class CharacterController;
	class GameObject;
}

namespace fq::client
{
	class DeadArmour;
	class GoddessStatue;
	class PlayerHPBar;

	class Soul :public game_module::Component
	{
	public:
		Soul();
		~Soul();

		void DestorySoul();
		fq::client::ESoulType GetSoulType() const { return mSoulType; }
		void SetSoulType(fq::client::ESoulType type);

		void SetSoulColor();

		void ReleaseGoddessStatue();

		float GetSoulHpRatio()const;
		float GetSoulGauge() const { return mSoulGauge; }
		void SetSoulGauge(float soulGauge) { mSoulGauge = soulGauge; }
		float GetMaxSoulGauge() const { return mMaxSoulGauge; }
		void SetMaxSoulGauge(float maxSoulGauge) { mMaxSoulGauge = maxSoulGauge; }
		float GetSoulGaugeRatio() const { if (mMaxSoulGauge == 0.f) { return 0; } return mSoulGauge / mMaxSoulGauge; }

		/// <summary>
		/// �ҿ��� �������·� �����մϴ�.
		/// </summary>
		void SetInvincible();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;
		void OnTriggerEnter(const fq::game_module::Collision& collision) override;
		void OnTriggerExit(const fq::game_module::Collision& collision) override;
		void OnUpdate(float dt) override;
		void OnLateUpdate(float dt) override;

		// ��ȥ �ֺ� ���� �÷��̾� �˻� �� �����Ÿ� �ȿ� ������ ����
		void checkOtherPlayer();
		// ��ȥ �α� ���� ��ȣ�ۿ�
		void selectArmour();
		// ��ȥ �α� ���Ż� ��ȣ�ۿ�
		void selectGoddessStatue(float dt);
		void checkReleaseGoddessStatue();
		// ��ȥ HP ������Ʈ
		void updateSoulHP(float dt);
		// ���� ���� ���� ��츦 üũ��
		bool handleOnSummon();

		// �ҿ� ���۽� hp ���� 
		void setSoulHP();
		void setName();

		void processInput(float dt);

	private:
		game_module::CharacterController* mController;
		std::vector<DeadArmour*> mSelectArmours;
		GoddessStatue* mSelectGoddessStatue;

		PlayerHPBar* mPlayerHpBar;
		BGaugeUI* mBGaugeUI;
		ESoulType mSoulType;

		/// ü�� ����
		// ��ó�� �÷��̾ �ִ���
		// ������ ���� �ִ���
		float mHP;
		bool mbIsDistanceInPlayer;
		bool mbIsInSafeZone;

		/// ���Ż� ����
		// ��ȥ ���Ż� ���� ������
		float mNeedHoldB;
		float mCurHoldB;
		bool mIsOverlayGoddessStatue;

		bool mbIsOnSummon;
		DeadArmour* mSummonArmourOrNull;

		// ��ȥ ������
		float mSoulGauge;
		float mMaxSoulGauge;

		// BGaugeUI ���Ż�� ���� ��ó�� �� Ȱ��ȭ�ϴ� ���� �ۼ��� ���� ������ ���� �߰�
		bool mbIsVisibleBGaugeUI;
		float mBGaugeFillAmount;

		// �ҿ� ��� ����
		float mDashSpeed;
		float mDashElapsed;
		float mDashCoolTime;

	private:
		friend void RegisterMetaData();
	};

}
