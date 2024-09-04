#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class ArmourSet : public game_module::Component
	{
	public:
		ArmourSet();
		~ArmourSet();

		std::vector<game_module::PrefabResource> GetArmourPrefabList() { return mArmourList; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		std::vector<game_module::PrefabResource> mArmourList;

		friend void RegisterMetaData();
	};
}
