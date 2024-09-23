#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class SoulManager : public fq::game_module::Component
	{
	public:
		SoulManager();
		~SoulManager();

		int GetPlayerDeathCount(unsigned int id);
		int GetbIsPlayerDeath(unsigned int id);
		void AddPlayerArmourDeathCount(unsigned int id);
		void SetbIsPlayerSoulDeath(unsigned int id, bool isDeath);

	private:
		void OnStart()override;
		void OnFixedUpdate(float dt) override;

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		std::vector<int> mPlayerArmourDeathCount;
		std::vector<bool> mbIsPlayerSoulDeath;

		friend void RegisterMetaData();
	};
}

