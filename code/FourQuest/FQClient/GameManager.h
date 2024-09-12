#pragma once

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/PrefabResource.h"

#include "ClientEvent.h"

namespace fq::client
{
	class GameManager : public fq::game_module::Component
	{
		using EventHandler = game_module::EventHandler;

	public:
		GameManager();
		GameManager(const GameManager& other);
		GameManager& operator=(const GameManager& other);
		~GameManager();

		/// <summary>
		/// 등록한 플레이어들을 반환합니다
		/// </summary>
		const std::vector<std::shared_ptr<game_module::GameObject>>& GetPlayers()const { return mPlayers; }

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override { return *this; }

		void OnUpdate(float dt) override;
		void OnStart() override;
		void OnAwake() override;
		void OnDestroy() override;

		void EventProcessOffPopupPause();
		void EventProcessOffPopupSetting();
		void testKey();

		// temp alpha
		void SavePlayerState();
		std::shared_ptr<game_module::GameObject> SpawnPlayer(fq::game_module::PrefabResource prefab, int index);

	private:
		EventHandler mRegisterPlayerHandler;

		// Pause UI Off 처리
		EventHandler mOffPopupSettingHandler;
		EventHandler mOffPopupPauseHandler;

		std::vector<std::shared_ptr<game_module::GameObject>> mPlayers;

		// pause 처리
		bool mIsStop;
		bool mIsAutoSpawn;
		fq::game_module::PrefabResource mPauseUI;

		game_module::PrefabResource mSoul;
		game_module::PrefabResource mKnight;
		game_module::PrefabResource mMagic;
		game_module::PrefabResource mArcher;
		game_module::PrefabResource mWarrior;

		friend void RegisterMetaData();
	};


}