#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class ScreenBlending : public game_module::Component
	{
	public:
		ScreenBlending() = default;
		~ScreenBlending() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

		void applyTextureBlending(bool bIsActive);

	private:
		game_module::GameObject* mPostProcessingOrNull;
		
		float mDuration;
		float mElapsed;
		std::string mTexturePath;
		std::wstring mWTexturePath;
		DirectX::SimpleMath::Color mStartBlendColor;
		DirectX::SimpleMath::Color mEndBlendColor;
		
		friend void RegisterMetaData();
	};
}