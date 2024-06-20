#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class MonsterSpawner : public fq::game_module::Component
	{
	public:
		MonsterSpawner();
		~MonsterSpawner();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnUpdate(float dt) override;

	private:
		float mSpawnCoolTime;
		float mSpawnElapsedTime;

		fq::game_module::PrefabResource mMonster;

		friend void RegisterMetaData();
	};


}