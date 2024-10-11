#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "PlayerDefine.h"
#include "BGaugeUI.h"

namespace fq::client
{
	class DeadArmour :	public game_module::Component
	{
	public:
		DeadArmour();
		~DeadArmour();
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	public:
		/// <summary>
		/// 죽은 갑옷에서 플레이어를 소환합니다
		/// </summary>
		bool SummonLivingArmour(PlayerInfo info);

		void SetUnequippedPlayerId(int playerID) { mUnequippedPlayerId = playerID; }
		int GetUnequippedPlayerId() { return mUnequippedPlayerId; }

		float GetPlayerArmourCoolTime() { return mPlayerArmourCoolTime; }

		/// <summary>
		/// 갑옷이 빙의 가능한 상태인지 설정합니다 
		/// </summary>
		void SetSummonAble(bool isSummonAble);

		bool IsSummonAble()const;

		/// <summary>
		/// 플레이어가 갑옷을 입을 수 있는 지 쿨타임을 확인하고 UI를 표시합니다.
		/// </summary>
		void CheckArmourCoolTime(unsigned int playerID) const;

		// 결과창 용 
		void AddArmourCount(int playerID);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;

		void setOutlineColor(DirectX::SimpleMath::Color color);
		void setUI(bool isVisible);
	private:
		game_module::PrefabResource mLivingArmourPrefab;

		unsigned short mPlayerCount;
		bool mbIsVisible;
		bool mbIsSummonAble;

		int mUnequippedPlayerId;
		float mPlayerArmourCoolTime;

		friend void RegisterMetaData();
	};
}