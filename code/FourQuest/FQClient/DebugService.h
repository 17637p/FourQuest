#pragma once

#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class TextUI;
}

namespace fq::client
{
	class DebugService : public game_module::Component
	{
	public:
		DebugService();
		~DebugService();

		void SetDebug(bool val);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;
		void OnUpdate(float dt) override;

		void updateUI();
		void updateInput();
		void savePlayerState();
		void resurrectAllSoul();

	private:
		bool mbOnDebugText;
		game_module::TextUI* mFPSUI;

	private:
		friend void RegisterMetaData();
	};


}