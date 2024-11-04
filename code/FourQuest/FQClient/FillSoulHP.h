#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/GameModuleEnum.h"

namespace fq::client
{
	class FillSoulHP : public game_module::Component
	{
	public:
		FillSoulHP();
		~FillSoulHP() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnTriggerStay(const game_module::Collision& collision) override;

	private:
		float mMinHP;
		float mFillHP;

		friend void RegisterMetaData();
	};
}