#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
}

namespace fq::client
{
	class LoadingUI : public game_module::Component
	{
	public:
		LoadingUI();
		~LoadingUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		game_module::ScreenManager* mScreenManager;
	};
}

