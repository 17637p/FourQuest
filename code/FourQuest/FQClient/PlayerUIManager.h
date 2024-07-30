#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	/// <summary>
	/// �÷��̾� ���� ���� HUD�� �����ϰ� �ڵ����� ��� ���� �����ִ� ������Ʈ 
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
		float mHeightRatio; // playerHUD ���� ��ġ�� ���� ���� 

		game_module::ScreenManager* mScreenManager;

		fq::game_module::PrefabResource mPlayerUIPrefab; 
		std::vector<std::shared_ptr<game_module::GameObject>> mPlayerUIs;

		friend void RegisterMetaData();
	};
}

