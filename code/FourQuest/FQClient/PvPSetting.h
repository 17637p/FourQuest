#pragma once

#include "../FQGameModule/Component.h"

namespace fq::client
{
	class PvPSetting : public game_module::Component
	{
	public:
		void OnStart() override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		bool mbIsOn = false;

		friend void RegisterMetaData();
	};

}
