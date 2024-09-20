#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class ArmourSpawner : public game_module::Component
	{
	public:
		ArmourSpawner();
		~ArmourSpawner();

		std::vector<game_module::PrefabResource> GetArmourPrefabList() { return mArmourList; }

		void SpawnArmour();

		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		std::vector<game_module::PrefabResource> mArmourList;

		friend void RegisterMetaData();
	};
}

