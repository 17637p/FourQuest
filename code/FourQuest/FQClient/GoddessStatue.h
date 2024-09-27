#pragma once

#include "../FQGameModule/Component.h"
#include <list>
namespace fq::client
{
	class Player;
	class Soul;
	class GoddessStatue : public fq::game_module::Component
	{
	public:
		GoddessStatue();
		~GoddessStatue();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		virtual void OnTriggerEnter(const game_module::Collision& collision) override;
		virtual void OnTriggerExit(const game_module::Collision& collision) override;

		// overlay �ϰڴٰ� ���´µ� �̹� ������ ��ȥ�� �ִٸ� false ��ȯ
		bool SetOverlaySoul(bool isOverlay, Soul* soul);
		bool GetIsCorrupt();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void DealingPlayer(float dt);
		void CleanUpGoddessState(float dt);
		void SpawnArmour();

	private:
		bool mIsCorrupt; // ������ ���Ż�����
		bool mIsOverlayedSoul; // ��ȥ ���� ������
		Soul* mCurOverlaySoul; // ���� ���� ���� soul

		std::list<Player*> mInRangePlayer; // �ݶ��̴� ���� �ȿ� �ִ� �÷��̾�
		float mCurGauge; // ��ȭ ����
		float mDealingCoolTime;

		// �ν����� ��
		float mDealingTime; // ��Ʈ�� �ִ� �ð� ����
		int mDealingDamage; // ���� ������
		//float mDealingRange; - �ݶ��̴��� ����
		float mMaxGauge; // ä���� �ϴ� ������
		float mFillGaugeSpeed; // 1�ʿ� ä������ ��
		float mDecreaseSpeed; // ��ȥ ��Ż ��, �����ϴ� ��

	private:
		friend void RegisterMetaData();
	};
}

// ������ ���¶��
// �浹 üũ�ؼ� �÷��̾ ������ ��Ʈ �� �ֱ� 
// �浹 üũ�ؼ� ��ȥ�� ���� �� ����
// ���� �Ѵٸ� ������ ä��� 
// ���� ���� ���� 
