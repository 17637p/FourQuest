#pragma once

#include "../FQGameModule/Component.h"

namespace fq::client
{
	class MonsterSpawnerDetector : public game_module::Component
	{
	public:
		MonsterSpawnerDetector() = default;
		~MonsterSpawnerDetector() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnTriggerEnter(const game_module::Collision& collision) override;
	};
}