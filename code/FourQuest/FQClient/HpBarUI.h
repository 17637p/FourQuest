#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class HpBarUI : public game_module::Component
	{
	public:
		HpBarUI();
		~HpBarUI();

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		bool IsVisible() const { return mIsVisible; }
		void SetVisible(bool val) { mIsVisible = val; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		void OnUpdate(float dt) override;

	private:
		bool mIsVisible;
	};


}