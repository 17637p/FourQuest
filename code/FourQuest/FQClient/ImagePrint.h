#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
}

namespace fq::client
{
	class ImagePrint : public fq::game_module::Component
	{
	public:
		ImagePrint();
		~ImagePrint();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleScreen();

	private:
		game_module::ScreenManager* mScreenManager;

		float mOffsetX;
		float mOffsetY;

	private:
		friend void RegisterMetaData();
	};
}
