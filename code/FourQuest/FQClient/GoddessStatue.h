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

		// overlay 하겠다고 보냈는데 이미 빙의한 영혼이 있다면 false 반환
		bool SetOverlaySoul(bool isOverlay, Soul* soul);
		bool GetIsCorrupt();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void DealingPlayer(float dt);
		void CleanUpGoddessState(float dt);
		void SpawnArmour();

	private:
		bool mIsCorrupt; // 오염된 여신상인지
		bool mIsOverlayedSoul; // 영혼 빙의 중인지
		Soul* mCurOverlaySoul; // 현재 빙의 중인 soul

		std::list<Player*> mInRangePlayer; // 콜라이더 범위 안에 있는 플레이어
		float mCurGauge; // 정화 정도
		float mDealingCoolTime;

		// 인스펙터 용
		float mDealingTime; // 도트딜 넣는 시간 간격
		int mDealingDamage; // 넣을 데미지
		//float mDealingRange; - 콜라이더로 조절
		float mMaxGauge; // 채워야 하는 게이지
		float mFillGaugeSpeed; // 1초에 채워지는 양
		float mDecreaseSpeed; // 영혼 이탈 시, 감소하는 양

	private:
		friend void RegisterMetaData();
	};
}

// 오염된 상태라면
// 충돌 체크해서 플레이어가 있으면 도트 뎀 주기 
// 충돌 체크해서 영혼이 있을 때 빙의
// 빙의 한다면 게이지 채우기 
// 보상 갑옷 생성 
