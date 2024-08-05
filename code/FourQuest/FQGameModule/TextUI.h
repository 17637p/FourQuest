#pragma once
#include "Component.h"

namespace fq::game_module
{
	class TextUI : public Component
	{
	public:
		TextUI();
		~TextUI();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
	};
}

