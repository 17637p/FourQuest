#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class GameOverHandler : public game_module::Component
	{
	public:
		GameOverHandler();
		~GameOverHandler() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnDestroy() override;

	private:
		bool mbIsSelect;
		float mInputStartTIme;
		float mElapsed;

		friend void RegisterMetaData();
	};
}