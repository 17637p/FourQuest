#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class Spawner : public game_module::Component
	{
	public:
		Spawner();
		~Spawner();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

		int GetID() { return mID; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		//인스펙터 용 변수
		int mID;

	private:
		friend void RegisterMetaData();
	};
}

