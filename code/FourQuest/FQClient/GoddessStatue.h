#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

#include <list>
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

		void dealingPlayer(float dt);
		void cleanUpGoddessState(float dt);
		void spawnArmour();
		void buffPlayer(Player* player);
		void releaseBuffPlayer(Player* player);

	private:
		bool mIsCorrupt; // ������ ���Ż�����
		bool mIsOverlayedSoul; // ��ȥ ���� ������
		Soul* mCurOverlaySoul; // ���� ���� ���� soul

		std::list<Player*> mInRangePlayers; // �ݶ��̴� ���� �ȿ� �ִ� �÷��̾�
		std::unordered_map<Player*, std::shared_ptr<game_module::GameObject>> mDebuffEffects;
		std::unordered_map<Player*, std::shared_ptr<game_module::GameObject>> mBuffEffects;
		float mCurGauge; // ��ȭ ����
		float mDealingCoolTime;

		// �ν����� ��
		float mDealingTime; // ��Ʈ�� �ִ� �ð� ����
		int mDealingDamage; // ���� ������
		//float mDealingRange; - �ݶ��̴��� ����
		float mMaxGauge; // ä���� �ϴ� ������
		float mFillGaugeSpeed; // 1�ʿ� ä������ ��
		float mDecreaseSpeed; // ��ȥ ��Ż ��, �����ϴ� ��

		// Buff
		float mGodDamageBuff;
		float mGodCoolTimeBuff;
		float mGodMoveBuff;

		// Effect
		game_module::PrefabResource mPlayerDebuff;
		game_module::PrefabResource mPlayerBuff;
		game_module::PrefabResource mBuff;

	private:
		friend void RegisterMetaData();
	};
}

// ������ ���¶��
// �浹 üũ�ؼ� �÷��̾ ������ ��Ʈ �� �ֱ� 
// �浹 üũ�ؼ� ��ȥ�� ���� �� ����
// ���� �Ѵٸ� ������ ä��� 
// ���� ���� ���� 
