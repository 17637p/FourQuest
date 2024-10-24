#pragma once
#include "../FQGameModule/Component.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::client
{
	class UIShaker : public game_module::Component
	{
	public:
		UIShaker() = default;
		~UIShaker() = default;

		void SetCount(size_t count) { mCount = count; }
		void AddCount(size_t count) { mCount += count; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		size_t mCount;
		float mDuration;
		float mElapsed;
		DirectX::SimpleMath::Vector2 mStartOffset;
		DirectX::SimpleMath::Vector2 mEndOffset;
		std::vector<fq::graphics::UIInfo> mUIInfos;

		friend void RegisterMetaData();
	};
}