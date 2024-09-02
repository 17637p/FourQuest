#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
	class ImageUI;
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

		void SetProgressBar(float progress);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		float mProgressBarMaxWidth;
		game_module::ImageUI* mProgressBar;
		game_module::Transform* mProgressSoul;

		game_module::ScreenManager* mScreenManager;
	};
}

