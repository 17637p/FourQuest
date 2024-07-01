#pragma once

#include "../FQGameModule/GameModule.h"
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
		void SummonLivingArmour(PlayerInfo info);

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

		friend void RegisterMetaData();
	};
}