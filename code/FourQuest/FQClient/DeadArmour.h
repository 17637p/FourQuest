#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"
#include "PlayerDefine.h"

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

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;

		void setUI(bool isVisible);
	private:
		game_module::PrefabResource mLivingArmourPrefab;

		unsigned short mPlayerCount;
		bool mbIsVisible;

		int mUnequippedPlayerId;
		float mPlayerArmourCoolTime;

		friend void RegisterMetaData();
	};
}