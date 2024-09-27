#pragma once

#include "../FQGameModule/Component.h"
#include "PlayerDefine.h"
#include "BGaugeUI.h"

namespace fq::game_module
{
	class CharacterController;
}

namespace fq::client
{
	class DeadArmour;
	class GoddessStatue;

	class Soul :public game_module::Component
	{
	public:
		Soul();
		~Soul();
		
		void DestorySoul();
		fq::client::ESoulType GetSoulType() const { return mSoulType; }
		void SetSoulType(fq::client::ESoulType type);
	
		void SetSoulColor();
		void SetSoulHP();

		void ReleaseGoddessStatue();

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

	private:
		game_module::CharacterController* mController;
		std::vector<DeadArmour*> mSelectArmours;
		GoddessStatue* mSelectGoddessStatue;

		BGaugeUI* mBGaugeUI;
		ESoulType mSoulType;

		float mHP;
		bool mbIsDistanceInPlayer;

		/// ���Ż� ����
		// ��ȥ ���Ż� ���� ������
		float mNeedHoldB;
		float mCurHoldB;
		bool mIsOverlayGoddessStatue;

	private:
		friend void RegisterMetaData();
	};

}
