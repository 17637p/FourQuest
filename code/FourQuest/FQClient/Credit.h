#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class ScreenManager;
}

namespace fq::client
{
	class Credit : public game_module::Component
	{
	public:
		Credit();
		~Credit();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void setScaleScreen();

	private:
		// Inspector
		float mSeconds;
		std::string mNextSceneName;

		// 
		float mCurTime;
		game_module::ScreenManager* mScreenManager;

	private:
		friend void RegisterMetaData();
	};
}
