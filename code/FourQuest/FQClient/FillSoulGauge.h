#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	class FillSoulGauge : public game_module::Component
	{
	public:
		FillSoulGauge();
		~FillSoulGauge() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnTriggerEnter(const game_module::Collision& collision) override;

	private:
		float mFillGauge;

		friend void RegisterMetaData();
	};
}