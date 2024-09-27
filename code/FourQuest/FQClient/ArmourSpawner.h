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

		// 몬스터, 항아리, 상자 등 
		void SpawnArmour();
		// 여신상 용
		void SpawnArmourAll();

		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		std::vector<game_module::PrefabResource> mArmourList;

		friend void RegisterMetaData();
	};
}

