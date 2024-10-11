#pragma once

#include "Player.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class Light;
	class Component;
}

namespace fq::client
{
	class DynamicLightHelper : public fq::game_module::Component
	{
	public:
		DynamicLightHelper();
		DynamicLightHelper(const DynamicLightHelper& other) = default;
		DynamicLightHelper& operator=(const DynamicLightHelper& other) = default;
		~DynamicLightHelper() = default;

		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		std::shared_ptr<fq::game_module::Component> Clone(std::shared_ptr<fq::game_module::Component> clone /* = nullptr */) const override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		float mMinIntensity;
		float mMaxIntensity;
		float mSpeed;
		float mAccumulationTime;

		fq::game_module::Light* mLightOrNull;

		friend void RegisterMetaData();
	};
}

