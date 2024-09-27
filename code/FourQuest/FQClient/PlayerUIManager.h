#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::game_module
{
	class ScreenManager;
}

namespace fq::client
{
	class GameManager;
	/// <summary>
	/// 플레이어 수에 따라 HUD를 생성하고 자동으로 가운데 정렬 시켜주는 오브젝트 
	/// </summary>
	class PlayerUIManager : public game_module::Component
	{
	public:
		PlayerUIManager();
		~PlayerUIManager();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		void AddPlayer(int playerID);
		void DeletePlayer(int playerID);
		void SetPlayerUIPosition();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		float mHeightRatio; // playerHUD 들이 위치할 세로 비율 

		game_module::ScreenManager* mScreenManager;
		GameManager* mGameManager;

		fq::game_module::PrefabResource mPlayerUIPrefab; 
		std::vector<std::shared_ptr<game_module::GameObject>> mPlayerUIs;

		friend void RegisterMetaData();
	};
}

