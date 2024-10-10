#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class MultiPlayerObjectCreator : public game_module::Component
	{
	public:
		MultiPlayerObjectCreator();
		~MultiPlayerObjectCreator();

		virtual void OnStart() override;

	private:
		void spawn();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		int mNum;
		game_module::PrefabResource mSpawnObjectPrefab;
		//std::vector<game_module::PrefabResource> mObjectList;

	private:
		friend void RegisterMetaData();
	};
}

