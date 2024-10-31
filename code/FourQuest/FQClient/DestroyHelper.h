#pragma once
#include "../FQGameModule/Component.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::client
{
	class DestroyHelper : public game_module::Component
	{
	public:
		DestroyHelper();
		~DestroyHelper() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		float mDuration;
		float mElapsed;

		friend void RegisterMetaData();
	};
}